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

計算を実行する前に `log/` ディレクトリを作成してください。計算結果または異常内容は、実行日の `log/YYYY-MM-DD.log` へ追記されます。

```bash
mkdir -p log
./build/calc add 1 2
./build/calc log 2026-10-16
```

正常ログは `HH:MM:SS | <左辺> <演算子> <右辺> | <結果>`、異常ログは `HH:MM:SS | ERROR | <異常内容>` の形式です。引数や計算の異常は標準エラーへ出力します。ログの読み書き異常は標準出力へ出力し、非0の終了コードを返します。

## 要求仕様

利害関係者要求を `docs/requirements/stakeholder/`、システム要求およびソフトウェア要求を `docs/requirements/systems/<system>/` で管理し、次のコマンドで検証します。

```bash
strictdoc export docs --output-dir build/strictdoc
```

## ライセンス

ライセンスは未指定です。
