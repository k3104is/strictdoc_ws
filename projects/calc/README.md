# calc

## 概要

C++17で実装した、2つの数値を四則演算するコマンドラインアプリです。
`add`、`sub`、`mul`、`div` をサポートします。

## ディレクトリ構成

```text
src/        製品コード
include/    公開インターフェース
test/       GoogleTestテスト
docs/       StrictDoc要求仕様
build/      ビルド生成物
```

## 開発環境

C++17、CMake 3.20以降、Ninja、GoogleTest、StrictDocを使用します。
GoogleTest 1.15.2はテスト有効時にCMake FetchContentで取得します。

## ビルド

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

## テスト

```bash
ctest --test-dir build --output-on-failure
```

## 実行

```bash
./build/calc <add|sub|mul|div> <lhs> <rhs>
```

例: `./build/calc add 1.5 2.25` は `3.75` を出力します。
引数の誤り、不正・非有限な数値、ゼロ除算、結果のオーバーフローは標準エラーにメッセージを出力し、非0の終了コードを返します。

## 要求仕様

利害関係者要求を `docs/requirements/stakeholder/`、システム要求およびソフトウェア要求を `docs/requirements/systems/<system>/` で管理し、次のコマンドで検証します。

```bash
strictdoc export docs --output-dir build/strictdoc
```

## ライセンス

ライセンスは未指定です。
