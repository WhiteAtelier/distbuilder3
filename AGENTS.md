# Copilot Agents Instructions

## 総合

- ソースコードのコメントや、その他ドキュメントは、基本的に日本語を使うものとします。
- ただし、クラス名や関数名などに対するコメントなどでは、クラス名や関数名に使われている英語は、そのまま英語として使います。
- 句点は "."、読点は "," とします。また句読点の後ろに文が続く場合は、その後ろには必ずスペースを空けてください。
    - 例: `ソースコードのコメントや, その他のドキュメントは, 基本的に日本語を使うものとします. しかし, 例外はあります.`
- コメント文や markdown など、改行について挙動上の意味を持たない場合は、基本的に1行に1文とします。
- 英単語と日本語の間にはスペースを置いて空間をあけてください。
    - 例: `この関数は args を指定することで, 戻り値が void になるオーバーロードがあります. `

## C++

- 名前空間は `roah::distb` にしてください。
- オブジェクト変数の初期化には、可能な限り `{}` を使ってください。
    - 例: `Hoge hoge { "aaa" };`
    - ただしリテラルの場合は代入初期化を使用できます。
        - 例: `int hoge = 2;`
    - もしも型推論が可能な代入の場合は `auto` を使うようにしてください。
        - ダメな例: `std::string str2 = str1;`
        - 良い例: `auto str2 = str1;`
- `const` を積極的に使ってください。変更されることのない変数には必ずつけてください。関数の引数もです。
- `move` が可能な場合はできる限り move してください。
- インクルードガードには `#ifdef ... #define ... #endif` のタイプを使用します。
    - インクルードガード名は、そのヘッダファイルのディレクトリ構造を参考にし、roah ディレクトリ以下の名前とヘッダファイル名を大文字スネークで連結し、 `#define ROAH_PATH_TO_DIR_HEADER_NAME_HPP` のようにします。
    - ディレクトリ名はスネークケースが基本で、アンダースコアはそのままインクルードガードの define にも反映します。
        - 例: roah/web_server/foo_bar.hpp -> `ROAH_WEB_SERVER_FOO_BAR_HPP`
- クラス定義について、クラス名は基本的にファイル名と一致させ、しかし PascalCase とします。
    - 例: foo_bar.hpp -> `class FooBar {...};`
- クラス定義について、基本的にすべてのクラスに、デフォルトコンストラクタ、コピーコンストラクタ、ムーブコンストラクタ、コピー代入、ムーブ代入、デストラクタを用意して、 delete の場合でも明示的に delete します。
    - 5-rules を明記します
    - ただしそれらの明記は、可能な限り cpp ファイル内で行います。
- 関数定義の引数は、関数内で move することがない限り、リテラルであっても const を適用します。
    - 例: `void foobar(const int argc) { ... }`
- そしてその関数宣言についても、仕様的には不要ですが、同じく const を適用します。
    - 例: `void foobar(const int argc);`
- クラスメソッドの定義において、クラスメンバにアクセスする際は、必ず `this->` を明記します。
- 関数定義における名前空間は、ひとつひとつについて完全名で記してください。
    - ダメな例: `namespace roah::hoge { void foobar() { ... /* 定義 */ ... } }`
    - 良い例: `void roah::hoge::foobar() { ... /* 定義 */ ... }`
- 必ず、メソッドの定義は、テンプレートなどの事情を除いて、可能な限り cpp ファイルに実装します。
    - inline は極力使用しません。
- 全体のフォーマットは clang-format で実行されるため、 AGENT は気にしなくてよい（ある程度フォーマットされた状態で良く、 clang-format rule に準拠する必要はない）です。

## 権利表記

- すべてのソースファイル、ヘッダファイルについて、完全に私が書いたもの、もしくは AI の助力によって書いたものについては、 MIT License で配布します。
    - 以下に示す権利表記を、ファイルの冒頭に書くことにします。
        - `Copyright (C) 2026 White Atelier`
        - `This software is released under the MIT License.`
        - `See the LICENSE file in the project root for more details.`
    - AI Agent が、ほとんどを作成するコードに、この権利表記を書いてはいけません。
- Claude Agent が書き出すコードには、以下に示す表記を、ファイル冒頭に書くことにします。
    - `This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.`
    - `The generated code is provided as-is.`
- 他の Agent service が書き出すコードには, 上記の表記を参考に、ファイルの冒頭に書いてください。
    - しかしソースコードには基本的に Claude を使っています。
