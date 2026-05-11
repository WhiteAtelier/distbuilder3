# Copyright (C) 2026 White Atelier
# This software is released under the MIT License.
# See the LICENSE file in the project root for more details.

cmake_minimum_required(VERSION 3.30)

project(distbuilder
    VERSION
        3.0.0
    DESCRIPTION
        "Roah Distbuilder"
    LANGUAGES
        CXX
)

include(cmake/doxygen/doxygen.cmake)
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

add_subdirectory(app)

add_doxygen_target()
