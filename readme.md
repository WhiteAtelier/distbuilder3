# distbuilder - Version 3

## recipes

[https://github.com/WhiteAtelier/distbuilder3-libraries](https://github.com/WhiteAtelier/distbuilder3-libraries)

## Usage

### Initialize / Check configuration

```bash
distbuilder --check-conf
```

### Build

```bash
distbuilder <SourceDir>
```

## 配布パッケージの生成

CMake の構成と Release ビルドを完了した後, 次のコマンドで配布パッケージを生成します.
`build` は実際のビルドディレクトリに置き換えてください.
単一構成の generator では, 構成時に `CMAKE_BUILD_TYPE=Release` を指定してください.

```bash
cpack --config build/CPackConfig.cmake -C Release
```

パッケージは tar.gz 形式で生成し, 出力先の既定値はプロジェクト直下の `cpack` ディレクトリです.
出力先は構成時に `DISTBUILDER3_CPACK_PACKAGE_DIRECTORY` で指定でき, `CMAKE_INSTALL_PREFIX` には依存しません.

```bash
cmake -S . -B build -DDISTBUILDER3_ENABLE_CPACK=ON -DDISTBUILDER3_CPACK_PACKAGE_DIRECTORY=S:/packages/distbuilder
```

パッケージ名にはバージョン, OS, architecture が含まれます.
例えば Linux の x86_64 ビルドでは, `distbuilder-3.4.0-Linux-x86_64.tar.gz` になります.
アーカイブには既存の `install()` 定義に従ってファイルを格納し, 実行ファイルは `bin` ディレクトリに配置します.

## License

MIT License

Copyright (c) 2026 Shirao Shotaro

依存ライブラリを除くこのプロジェクト内のすべてのリソースは MIT License を適用します.

このソフトウェアの使用により生じるいかなる損害についても, 作者は責任を負いません.
