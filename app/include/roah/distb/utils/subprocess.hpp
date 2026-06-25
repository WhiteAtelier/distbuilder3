// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_UTILS_SUBPROCESS_HPP
#define ROAH_DISTB_UTILS_SUBPROCESS_HPP

#include <string>
#include <vector>

namespace roah::distb::utils {

// run() の戻り値を保持する struct.
struct RunResult
{
    // プロセスの終了コード.
    int         exit_code;
    // 標準出力のキャプチャ文字列.
    std::string stdout_output;
    // 標準エラーのキャプチャ文字列.
    std::string stderr_output;
};

struct RunArgs
{
    bool print_stdout   = true;  // 標準出力を cout に書き出すかどうか.
    bool print_stderr   = true;  // 標準エラーを cerr に書き出すかどうか.
    bool capture_stdout = true;  // 標準出力をキャプチャするかどうか.
    bool capture_stderr = true;  // 標準エラーをキャプチャするかどうか.
};

// 指定されたコマンドを実行し, 結果を返す.
// stdout はリアルタイムで cout に, stderr は cerr に書き出される.
// この関数はブロッキングで, プロセスの終了まで待機する.
RunResult
run(const std::vector<std::u8string> & cmds, const RunArgs & args = {});

}  // namespace roah::distb::utils

#endif  // ROAH_DISTB_UTILS_SUBPROCESS_HPP
