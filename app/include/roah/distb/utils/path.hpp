#ifndef ROAH_DISTB_UTILS_PATH_HPP
#define ROAH_DISTB_UTILS_PATH_HPP

#include <filesystem>

namespace roah::distb::utils {

/// @brief 指定されたファイルパスを, 絶対パスかつ platform native に正規化する
std::filesystem::path
makeAbsolutePath(const std::filesystem::path & path);

/// @brief child が parent のサブディレクトリかどうかを判定する
///
/// どちらのパスも, makeAbsolutePath() によって, 絶対パスかつ platform native に正規化されている必要がある.
///
/// @param parent
///     絶対パスで正規化された親ディレクトリパス.
///
/// @param child
///     絶対パスで正規化された子ディレクトリパス.
///
/// @param when_same
///     parent と child が同じパスの場合の戻り値.
///
/// @return
///     parent が child のサブディレクトリである場合は true, それ以外の場合は false.
bool
isSubDirectory(const std::filesystem::path & parent, const std::filesystem::path & child, const bool when_same = true);

}  // namespace roah::distb::utils

#endif
