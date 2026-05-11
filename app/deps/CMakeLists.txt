# This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
# The generated code is provided as-is.

set(YAML_CPP_REPO_NAME "yaml-cpp")
set(NLOHMANN_JSON_REPO_NAME "json")
set(CLI11_REPO_NAME "CLI11")

# deps のビルド作業用ベースディレクトリ.
set(DEPS_BUILD_BASE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/.build"
    CACHE PATH "deps のビルド作業用ベースディレクトリ"
)

# ソースディレクトリとビルドディレクトリ.
# デフォルトは DEPS_BUILD_BASE_DIR/${REPO_NAME} 以下.
set(YAML_CPP_SOURCE_DIR "${DEPS_BUILD_BASE_DIR}/${YAML_CPP_REPO_NAME}")
set(YAML_CPP_BINARY_DIR "${DEPS_BUILD_BASE_DIR}/${YAML_CPP_REPO_NAME}/build")

set(NLOHMANN_JSON_SOURCE_DIR "${DEPS_BUILD_BASE_DIR}/${NLOHMANN_JSON_REPO_NAME}")
set(NLOHMANN_JSON_BINARY_DIR "${DEPS_BUILD_BASE_DIR}/${NLOHMANN_JSON_REPO_NAME}/build")

set(CLI11_SOURCE_DIR "${DEPS_BUILD_BASE_DIR}/${CLI11_REPO_NAME}")
set(CLI11_BINARY_DIR "${DEPS_BUILD_BASE_DIR}/${CLI11_REPO_NAME}/build")

include(FetchContent)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        yaml-cpp-0.9.0
    SOURCE_DIR     "${YAML_CPP_SOURCE_DIR}"
    BINARY_DIR     "${YAML_CPP_BINARY_DIR}"
)

# yaml-cpp のテストや util は不要なので無効化.
set(YAML_CPP_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS   OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(yaml-cpp)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    SOURCE_DIR     "${NLOHMANN_JSON_SOURCE_DIR}"
    BINARY_DIR     "${NLOHMANN_JSON_BINARY_DIR}"
)

# nlohmann/json のテストや benchmark は不要なので無効化.
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.6.2
    SOURCE_DIR     "${CLI11_SOURCE_DIR}"
    BINARY_DIR     "${CLI11_BINARY_DIR}"
)

# CLI11 のテストや例は不要なので無効化.
set(CLI11_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
set(CLI11_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CLI11_BUILD_DOCS     OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(CLI11)
