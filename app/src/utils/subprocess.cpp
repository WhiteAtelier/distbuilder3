// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <roah/distb/utils/subprocess.hpp>

#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#    include <windows.h>
#else
#    include <sys/wait.h>
#    include <unistd.h>
#endif

// 無名名前空間: パイプ読み取りのヘルパー関数.
namespace {

#ifdef _WIN32

// UTF-8 文字列を UTF-16 wstring に変換する.
std::wstring u8ToWide(const std::u8string& u8str) {
    if (u8str.empty()) {
        return {};
    }
    const int len = MultiByteToWideChar(
        CP_UTF8, 0,
        reinterpret_cast<const char*>(u8str.data()),
        static_cast<int>(u8str.size()),
        nullptr, 0
    );
    std::wstring wide(len, L'\0');
    MultiByteToWideChar(
        CP_UTF8, 0,
        reinterpret_cast<const char*>(u8str.data()),
        static_cast<int>(u8str.size()),
        wide.data(), len
    );
    return wide;
}

// Windows 用: HANDLE からデータを読み取り, stream に書き出しながらキャプチャした文字列を返す.
std::u8string readPipeToStream(const HANDLE pipe, std::ostream& stream) {
    std::u8string captured;
    char buffer[4096];
    DWORD bytes_read = 0;
    while (ReadFile(pipe, buffer, sizeof(buffer), &bytes_read, nullptr) && bytes_read > 0) {
        const auto offset = captured.size();
        captured.resize(offset + bytes_read);
        std::memcpy(captured.data() + offset, buffer, bytes_read);
        stream.write(buffer, bytes_read);
        stream.flush();
    }
    return captured;
}

// Windows コマンドライン向けに, 引数を適切にクォートして wstring に追記する.
void appendQuotedArgW(std::wstring& cmdline, const std::u8string& arg) {
    const auto wide = u8ToWide(arg);
    // スペース, ダブルクォート, タブを含む場合はクォートで囲む.
    const bool needs_quote = wide.empty() || wide.find_first_of(L" \t\"") != std::wstring::npos;
    if (!needs_quote) {
        cmdline += wide;
        return;
    }
    cmdline += L'"';
    for (const wchar_t c : wide) {
        if (c == L'"') {
            cmdline += L"\\\"";
        } else {
            cmdline += c;
        }
    }
    cmdline += L'"';
}

#else

// Unix 用: ファイルディスクリプタからデータを読み取り, stream に書き出しながらキャプチャした文字列を返す.
std::u8string readFdToStream(const int fd, std::ostream& stream) {
    std::u8string captured;
    char buffer[4096];
    ssize_t bytes_read = 0;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        const auto offset = captured.size();
        captured.resize(offset + static_cast<size_t>(bytes_read));
        std::memcpy(captured.data() + offset, buffer, static_cast<size_t>(bytes_read));
        stream.write(buffer, bytes_read);
        stream.flush();
    }
    return captured;
}

#endif

} // namespace

roah::distb::utils::RunResult roah::distb::utils::run(const std::vector<std::u8string>& cmds) {
    RunResult result;

    if (cmds.empty()) {
        result.exit_code = -1;
        return result;
    }

#ifdef _WIN32

    // コマンドライン文字列を構築する (UTF-16).
    std::wstring cmdline;
    for (const auto& arg : cmds) {
        if (!cmdline.empty()) {
            cmdline += L' ';
        }
        appendQuotedArgW(cmdline, arg);
    }

    // stdout/stderr 用のパイプを作成する.
    SECURITY_ATTRIBUTES sa {};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    HANDLE stdout_read = nullptr;
    HANDLE stdout_write = nullptr;
    HANDLE stderr_read = nullptr;
    HANDLE stderr_write = nullptr;

    if (!CreatePipe(&stdout_read, &stdout_write, &sa, 0)) {
        result.exit_code = -1;
        return result;
    }
    // 親プロセス側の読み取りハンドルを継承不可にする.
    SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0);

    if (!CreatePipe(&stderr_read, &stderr_write, &sa, 0)) {
        CloseHandle(stdout_read);
        CloseHandle(stdout_write);
        result.exit_code = -1;
        return result;
    }
    SetHandleInformation(stderr_read, HANDLE_FLAG_INHERIT, 0);

    // プロセスを作成する.
    STARTUPINFOW si {};
    si.cb = sizeof(STARTUPINFOW);
    si.hStdOutput = stdout_write;
    si.hStdError = stderr_write;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi {};
    const BOOL created = CreateProcessW(
        nullptr,
        cmdline.data(),
        nullptr,
        nullptr,
        TRUE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    // 子プロセス側の書き込みハンドルを閉じる (EOF が検出できるようにする).
    CloseHandle(stdout_write);
    CloseHandle(stderr_write);

    if (!created) {
        CloseHandle(stdout_read);
        CloseHandle(stderr_read);
        result.exit_code = static_cast<int>(GetLastError());
        return result;
    }

    CloseHandle(pi.hThread);

    // スレッドを立ち上げて stdout/stderr をリアルタイムに読み取る.
    std::u8string stdout_captured;
    std::u8string stderr_captured;

    // スレッドをスコープで囲み, スコープを抜けると自動的に join される.
    {
        std::jthread stdoutThread { [&]() {
            stdout_captured = readPipeToStream(stdout_read, std::cout);
        } };
        std::jthread stderrThread { [&]() {
            stderr_captured = readPipeToStream(stderr_read, std::cerr);
        } };
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    CloseHandle(pi.hProcess);
    CloseHandle(stdout_read);
    CloseHandle(stderr_read);

    result.exit_code = static_cast<int>(exit_code);
    result.stdout_output = std::move(stdout_captured);
    result.stderr_output = std::move(stderr_captured);

#else

    // stdout/stderr 用のパイプを作成する.
    int stdout_pipe[2] = { -1, -1 };
    int stderr_pipe[2] = { -1, -1 };

    if (pipe(stdout_pipe) != 0) {
        result.exit_code = -1;
        return result;
    }
    if (pipe(stderr_pipe) != 0) {
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        result.exit_code = -1;
        return result;
    }

    // execvp に渡す argv を構築する.
    std::vector<char*> argv;
    argv.reserve(cmds.size() + 1);
    for (const auto& arg : cmds) {
        // execvp は char* を要求するが, 書き換えは行われない. u8string の data を reinterpret_cast する.
        argv.push_back(const_cast<char*>(reinterpret_cast<const char*>(arg.data())));
    }
    argv.push_back(nullptr);

    const pid_t pid = fork();
    if (pid < 0) {
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        result.exit_code = -1;
        return result;
    }

    if (pid == 0) {
        // 子プロセス: stdout/stderr をパイプの書き込み側に繋ぐ.
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);

        execvp(argv[0], argv.data());
        // execvp が失敗した場合.
        _exit(127);
    }

    // 親プロセス: 書き込み側のパイプを閉じる (子プロセス終了時に EOF が検出できるようにする).
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    // スレッドを立ち上げて stdout/stderr をリアルタイムに読み取る.
    std::u8string stdout_captured;
    std::u8string stderr_captured;

    // スレッドをスコープで囲み, スコープを抜けると自動的に join される.
    {
        std::jthread stdoutThread { [&]() {
            stdout_captured = readFdToStream(stdout_pipe[0], std::cout);
        } };
        std::jthread stderrThread { [&]() {
            stderr_captured = readFdToStream(stderr_pipe[0], std::cerr);
        } };
    }

    close(stdout_pipe[0]);
    close(stderr_pipe[0]);

    int status = 0;
    waitpid(pid, &status, 0);

    result.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    result.stdout_output = std::move(stdout_captured);
    result.stderr_output = std::move(stderr_captured);

#endif

    return result;
}
