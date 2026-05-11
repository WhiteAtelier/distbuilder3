# Copyright (C) 2026 White Atelier
# This software is released under the MIT License.
# See the LICENSE file in the project root for more details.

cmake_minimum_required(VERSION 3.30)

project(libroah
    VERSION
        1.3.0
    DESCRIPTION
        "Roah Common Libraries"
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

option(BUILD_ROAH_ASSERT "Build libroah::roah-assert" ON)
option(BUILD_ROAH_SUBPROCESS "Build libroah::roah-subprocess" ON)
option(BUILD_ROAH_LOGGER "Build libroah::roah-logger" ON)
option(BUILD_ROAH_CONFIG "Build libroah::roah-config" ON)
option(BUILD_ROAH_STRING "Build libroah::roah-string" ON)
option(BUILD_ROAH_URL_PARSER "Build libroah::roah-url-parser" ON)
option(BUILD_APP_ROAH_LOGGING_WEB_VIEWER "Build logging-web-viewer app" ON)

# Logger: Enable WebV sink
option(ROAH_LOGGER_ENABLE_WEBV_SINK "Enable WebV sink in roah-logger library" ON)

# --- Dependencies ---
if (BUILD_ROAH_ASSERT)
    # No dependencies for now
endif()

if (BUILD_ROAH_SUBPROCESS)
    # No dependencies for now
endif()

if (BUILD_ROAH_LOGGER)
    if (NOT BUILD_ROAH_SUBPROCESS)
        message(FATAL_ERROR "roah-logger library requires roah-subprocess library. Please enable BUILD_ROAH_SUBPROCESS option.")
    endif()

    find_package(spdlog 1 REQUIRED)

    if (ROAH_LOGGER_ENABLE_WEBV_SINK)
        find_package(ixwebsocket REQUIRED)
        find_package(nlohmann_json REQUIRED)
    endif()
endif()

if (BUILD_ROAH_CONFIG)
    find_package(toml11 REQUIRED)
endif()

if (BUILD_ROAH_STRING)
    # No dependencies for now
endif()

if (BUILD_ROAH_URL_PARSER)
    # No dependencies for now
endif()

if (BUILD_APP_ROAH_LOGGING_WEB_VIEWER)
    # Require roah libraries...
    # - roah-assert
    # - roah-url-parser
    # - roah-logger
    # - roah-config
    if (NOT BUILD_ROAH_ASSERT)
        message(FATAL_ERROR "logging-web-viewer app requires roah-assert library. Please enable BUILD_ROAH_ASSERT option.")
    endif()
    if (NOT BUILD_ROAH_URL_PARSER)
        message(FATAL_ERROR "logging-web-viewer app requires roah-url-parser library. Please enable BUILD_ROAH_URL_PARSER option.")
    endif()
    if (NOT BUILD_ROAH_LOGGER)
        message(FATAL_ERROR "logging-web-viewer app requires roah-logger library. Please enable BUILD_ROAH_LOGGER option.")
    endif()
    if (NOT BUILD_ROAH_CONFIG)
        message(FATAL_ERROR "logging-web-viewer app requires roah-config library. Please enable BUILD_ROAH_CONFIG option.")
    endif()

    find_program(NPM_EXECUTABLE npm REQUIRED)
    find_package(CLI11 REQUIRED)
    find_package(ixwebsocket REQUIRED)
    find_package(nlohmann_json REQUIRED)
endif()

add_subdirectory(libs)
add_subdirectory(apps)

add_doxygen_target()
export_config()

install(
    FILES
        "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION
        "lib/cmake/${CMAKE_PROJECT_NAME}"
    RENAME
        "libroah-LICENSE"
)
