// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_CONFJG_LIBRARY_HPP
#define ROAH_DISTB_CONFJG_LIBRARY_HPP

#include <vector>

#include <roah/distb/confjg/library_entry.hpp>

namespace roah::distb::confjg {

// ライブラリ設定ファイル (json.jsonc) のルート構造を表すクラス.
class Library {
public:
    // バージョンエントリのリスト.
    // 先頭エントリは完全な定義, 以降のエントリは base フィールドによる差分定義が可能.
    std::vector<LibraryEntry> entries;

    Library() = default;
    Library(const Library&) = default;
    Library(Library&&) = default;
    Library& operator=(const Library&) = default;
    Library& operator=(Library&&) = default;
    ~Library() = default;
};

} // namespace roah::distb::confjg

#endif // ROAH_DISTB_CONFJG_LIBRARY_HPP
