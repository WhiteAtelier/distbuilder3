# Copyright (C) 2026 White Atelier
# This software is released under the MIT License.
# See the LICENSE file in the project root for more details.

cmake_minimum_required(VERSION 3.30)

project(distbuilder
    VERSION
        3.2.0
    DESCRIPTION
        "Roah Distbuilder"
    LANGUAGES
        CXX
)

option(DISTBUILDER3_BUILD_TESTS "Build test project." OFF)

include(cmake/utils.cmake)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Debug postfix")

add_compile_options(
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>"
    "$<$<CXX_COMPILER_ID:MSVC>:/MP>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/WX>"

    "$<$<CXX_COMPILER_ID:GNU>:-Wall>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wextra>"
    "$<$<CXX_COMPILER_ID:GNU>:-Werror>"
)
add_compile_definitions(
    "$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>"
    "$<$<CXX_COMPILER_ID:MSVC>:WIN32_LEAN_AND_MEAN>"
)

add_subdirectory(deps)
find_package(nlohmann_json 3 REQUIRED)
find_package(toml11        4 REQUIRED)
find_package(CLI11         2 REQUIRED)

if (DISTBUILDER3_BUILD_TESTS)
    enable_testing()
    find_package(GTest 1 REQUIRED)
    include(GoogleTest)
endif()

add_subdirectory(app)
