# <プロジェクト名>

## 概要

このプロジェクトの目的を簡潔に記載する。

* 何を実現するプロジェクトか
* 主な機能
* 対象環境
* 対象ユーザー

## ディレクトリ構成

```text
.
├── src/            # ソースコード
├── include/        # 公開ヘッダ
├── test/           # テストコード
├── docs/           # ドキュメント・要求仕様
├── scripts/        # 開発支援スクリプト
├── build/          # ビルド生成物
├── CMakeLists.txt
├── README.md
└── AGENTS.md
```

プロジェクト固有のディレクトリを追加した場合は、この一覧も更新する。

## 開発環境

主な開発ツール:

* C++
* GCC
* CMake
* Ninja
* GoogleTest
* Git
* StrictDoc
* Codex CLI

必要に応じてバージョンを記載する。

## ビルド

標準ビルド:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Debugビルド

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build
```

### Releaseビルド

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build
```

## テスト

すべてのテストを実行する。

```bash
ctest --test-dir build --output-on-failure
```

特定のテストのみ実行する場合:

```bash
ctest --test-dir build -R <test-name> --output-on-failure
```

## 実行

```bash
./build/<executable>
```

必要に応じて引数や設定方法を記載する。

## コードフォーマット・静的解析

使用するツールと実行方法を記載する。

例:

```bash
clang-format
clang-tidy
```

## ドキュメント

設計資料・要求仕様などは `docs/` 以下で管理する。

例:

```text
docs/
├── requirements/
├── architecture/
└── design/
```

StrictDocを使用する場合:

```bash
strictdoc export docs
```

## 開発フロー

基本的な作業手順:

1. 関連する既存コード・仕様を確認する
2. 必要な変更を実装する
3. ビルドする
4. 関連するテストを実行する
5. 必要に応じてドキュメントを更新する
6. `git diff` で変更内容を確認する
7. `git status` で意図しない変更がないことを確認する

## 主要な設定ファイル

| ファイル             | 用途              |
| ---------------- | --------------- |
| `CMakeLists.txt` | ビルド設定           |
| `AGENTS.md`      | AIエージェント向け作業ルール |
| `.gitignore`     | Git管理対象外設定      |
| `.clang-format`  | コードフォーマット設定     |
| `strictdoc.toml` | StrictDoc設定     |

プロジェクト固有の設定ファイルがある場合は追加する。

## ライセンス

<ライセンスを記載>
