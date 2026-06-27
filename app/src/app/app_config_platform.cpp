#include "app_config.hpp"
//
#include "roah/distb/errors.hpp"

#include <array>
#include <filesystem>

#if defined(ROAH_ARCH_WIN32)
#    include <Windows.h>
//
#    include <ShlObj.h>
#else
#    error "Unsupported platform"
#endif

std::filesystem::path
roah::distb::app::AppConfig::_getDefaultFilePath()
{
#if defined(ROAH_ARCH_WIN32)
    PWSTR psz_path = NULL;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &psz_path)))
    {
        throw AppError{ "Failed to get User Profile path." };
    }
    auto ret = std::filesystem::path{ psz_path } / "distbuilder.conf";
    CoTaskMemFree(psz_path);
    return ret;
#else
    static_assert(false && "Unsupported platform");
#endif
}

std::filesystem::path
roah::distb::app::AppConfig::_getDefaultBuildDirectory()
{
#if defined(ROAH_ARCH_WIN32)
    auto         size = ::GetTempPath2W(0, nullptr);
    std::wstring path(static_cast<std::size_t>(size), L'\0');
    size = ::GetTempPath2W(size, path.data());
    path.resize(static_cast<std::size_t>(size));
    return std::filesystem::path{ path } / L"distb_buildtmp";
#else
    return std::filesystem::temp_directory_path() / "distb_buildtmp";
#endif
}

std::filesystem::path
roah::distb::app::AppConfig::_getDefaultInstallDirectory()
{
#if defined(ROAH_ARCH_WIN32)
    PWSTR psz_path = NULL;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &psz_path)))
    {
        throw AppError{ "Failed to get AppData path." };
    }
    auto ret = std::filesystem::path{ psz_path } / "distb";
    CoTaskMemFree(psz_path);
    return ret;
#else
    static_assert(false && "Unsupported platform");
#endif
}

std::filesystem::path
roah::distb::app::AppConfig::_getAppStoragePath()
{
#if defined(ROAH_ARCH_WIN32)
    PWSTR psz_path = NULL;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &psz_path)))
    {
        throw AppError{ "Failed to get AppData path." };
    }
    auto ret = std::filesystem::path{ psz_path } / "distbuilder";
    CoTaskMemFree(psz_path);
    return ret;
#else
    static_assert(false && "Unsupported platform");
#endif
}
