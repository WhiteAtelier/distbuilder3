# Copyright (C) 2026 White Atelier
# This software is released under the MIT License.
# See the LICENSE file in the project root for more details.

option(BUILD_TESTS "Build test project." OFF)

if (BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
endif()

# =============================================================================================== #
#
# roah_add_library
#
# - LIBRARIES (multi)
#   PRIVATE にリンクするライブラリ.
#
# - HEADERS (multi)
#   PUBLIC なヘッダファイル.
#
# - SOURCES (multi)
#   ソースファイル.
#
# - TESTS (multi)
#   テスト用のソースファイル.
#
# - COMPILE_DEFINITIONS (multi)
#   PRIVATE なコンパイル定数.
#
# - PUBLIC_COMPILE_DEFINITIONS (multi)
#   PUBLIC なコンパイル定数. 衝突に注意.
#
# - TEST_LIBRARIES (multi)
#   テスト用にのみリンクするライブラリ.
#
# - MSVC_SOURCES_FILTER_ROOT (single)
#   ソースファイルにおいて, この引数で指定したディレクトリ以下を MSVC のフィルタルートにします.
#
# - MSVC_HEADERS_FILTER_ROOT (single)
#   ヘッダファイルにおいて, この引数で指定したディレクトリ以下を MSVC のフィルタルートにします.
#
# - TARGET_FOLDER (single)
#   フィルタツリーにおいて, ライブラリを置く場所を指定する.
#
# =============================================================================================== #
function(roah_add_library ARG_TARGET_NAME)

    # --- function args ---
    set(options )
    set(oneValueArgs
        MSVC_SOURCES_FILTER_ROOT
        MSVC_HEADERS_FILTER_ROOT
        TARGET_FOLDER
    )
    set(multiValueArgs
        SOURCES
        HEADERS
        TESTS
        LIBRARIES
        COMPILE_DEFINITIONS
        PUBLIC_COMPILE_DEFINITIONS
        TEST_LIBRARIES
    )
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # --- target folder ---
    set(TARGET_FOLDER "libs")
    if (DEFINED ARG_TARGET_FOLDER)
        set(TARGET_FOLDER "${ARG_TARGET_FOLDER}/libs")
    endif()

    set(TEST_TARGET_FOLDER "tests")
    if (DEFINED ARG_TARGET_FOLDER)
        set(TEST_TARGET_FOLDER "${ARG_TARGET_FOLDER}/tests")
    endif()

    # --- SOURCES の一覧を絶対パスに変換 ---
    if (DEFINED ARG_SOURCES)
        set(ARG_ABS_SOURCES )
        foreach (rel_source_file IN LISTS ARG_SOURCES)
            if (IS_ABSOLUTE "${rel_source_file}")
                # 絶対パスはダメ.
                message(SEND_ERROR "Absolute path is not supported. ${rel_source_file} (Target: ${ARG_TARGET_NAME})")
            else()
                file(REAL_PATH "src/${rel_source_file}" abs_source_file)
                list(APPEND ARG_ABS_SOURCES "${abs_source_file}")
                message(DEBUG "SOURCE: ${abs_source_file}")
            endif()
        endforeach()
    endif()

    # --- HEADERS の一覧を絶対パスに変換 ---
    if (DEFINED ARG_HEADERS)
        set(ARG_ABS_HEADERS )
        foreach (rel_header_file IN LISTS ARG_HEADERS)
            if (IS_ABSOLUTE "${rel_header_file}")
                # 絶対パスはダメ.
                message(SEND_ERROR "Absolute path is not supported. ${rel_header_file} (Target: ${ARG_TARGET_NAME})")
            else()
                file(REAL_PATH "include/${rel_header_file}" abs_header_file)
                list(APPEND ARG_ABS_HEADERS "${abs_header_file}")
                message(DEBUG "HEADER: ${abs_header_file}")
            endif()
        endforeach()
    endif()

    # --- Add library ---
    add_library(${ARG_TARGET_NAME} STATIC)
    set_target_properties(${ARG_TARGET_NAME}
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            FOLDER "${TARGET_FOLDER}"
    )

    # --- Link libraries ---
    if (DEFINED ARG_LIBRARIES)
        target_link_libraries(${ARG_TARGET_NAME}
            PRIVATE
                ${ARG_LIBRARIES}
        )
    endif()

    # --- Compile definitions
    target_compile_definitions(${ARG_TARGET_NAME}
        PRIVATE
            # Debug Mode Flag
            $<$<CONFIG:Debug>:ROAH_DEBUG=1>

            # Architecture Flags
            $<$<PLATFORM_ID:Windows>:ROAH_ARCH_WIN32>
            $<$<PLATFORM_ID:Linux>:ROAH_ARCH_LINUX>
    )
    if (DEFINED ARG_COMPILE_DEFINITIONS)
        target_compile_definitions(${ARG_TARGET_NAME}
            PRIVATE
                ${ARG_COMPILE_DEFINITIONS}
        )
    endif()
    if (DEFINED ARG_PUBLIC_COMPILE_DEFINITIONS)
        target_compile_definitions(${ARG_TARGET_NAME}
            PUBLIC
                ${ARG_PUBLIC_COMPILE_DEFINITIONS}
        )
    endif()

    # --- Sources
    if (DEFINED ARG_ABS_SOURCES)
        target_sources(${ARG_TARGET_NAME}
            PRIVATE
                ${ARG_ABS_SOURCES}
        )
    endif()

    # --- Headers
    if (DEFINED ARG_ABS_HEADERS)
        target_sources(${ARG_TARGET_NAME}
            PUBLIC
                FILE_SET
                    HEADERS
                TYPE
                    HEADERS
                BASE_DIRS
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                FILES
                    ${ARG_ABS_HEADERS}
        )
        regist_doxygen_files("${ARG_ABS_HEADERS}")
    endif()

    # --- MSVC Filters
    if (MSVC)
        # ARG_SOURCES を見て, 相対パスである場合は src/ を付加する
        if (DEFINED ARG_SOURCES)
            if (DEFINED ARG_MSVC_SOURCES_FILTER_ROOT)
                string(REPLACE "/" "\\" source_filter_root "${ARG_MSVC_SOURCES_FILTER_ROOT}")
            else()
                set(source_filter_root "src")
            endif()

            foreach (rel_source_file IN LISTS ARG_SOURCES)
                # 絶対パス
                file(REAL_PATH "src/${rel_source_file}" abs_source_file)

                # フィルタ文字列
                string(REPLACE "/" "\\" rel_source_file "${rel_source_file}")
                string(REPLACE "${source_filter_root}\\" "" rel_source_file "${rel_source_file}")
                cmake_path(GET rel_source_file PARENT_PATH rel_source_file)
                source_group("${rel_source_file}" FILES "${abs_source_file}")
                message(DEBUG "FILTER: ${rel_source_file} -> ${abs_source_file}")
            endforeach()
        endif()

        # ARG_HEADERS を見て, include/ を付加する
        if (DEFINED ARG_HEADERS)
            if (DEFINED ARG_MSVC_HEADERS_FILTER_ROOT)
                string(REPLACE "/" "\\" headers_filter_root "${ARG_MSVC_HEADERS_FILTER_ROOT}")
            else()
                set(headers_filter_root "include\\${CMAKE_PROJECT_NAME}")
            endif()

            foreach (rel_header_file IN LISTS ARG_HEADERS)
                # 絶対パス
                file(REAL_PATH "include/${rel_header_file}" abs_header_file)

                # フィルタ文字列
                string(REPLACE "/" "\\" rel_header_file "${rel_header_file}")
                string(REPLACE "${headers_filter_root}\\" "" rel_header_file "${rel_header_file}")
                cmake_path(GET rel_header_file PARENT_PATH rel_header_file)
                source_group("${rel_header_file}" FILES "${abs_header_file}")
                message(DEBUG "FILTER: ${rel_header_file} -> ${abs_header_file}")
            endforeach()
        endif()
    endif()

    install(
        TARGETS ${ARG_TARGET_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}-export
        FILE_SET HEADERS
    )

    # ========================================= #
    # test
    # ========================================= #
    if (BUILD_TESTS AND (DEFINED ARG_TESTS))

        # --- TESTS の一覧を絶対パスに変換 ---
        if (DEFINED ARG_TESTS)
            set(ARG_ABS_TESTS )
            foreach (rel_test_file IN LISTS ARG_TESTS)
                if (IS_ABSOLUTE "${rel_test_file}")
                    message(SEND_ERROR "Absolute path is not supported. ${rel_test_file} (Target: ${ARG_TARGET_NAME})")
                else()
                    file(REAL_PATH "test/${rel_test_file}" abs_test_file)
                    list(APPEND ARG_ABS_TESTS "${abs_test_file}")
                    message(DEBUG "TEST: ${abs_test_file}")
                endif()
            endforeach()
        endif()

        # --- Add executable ---
        add_executable(${ARG_TARGET_NAME}-test)
        set_target_properties(${ARG_TARGET_NAME}-test
            PROPERTIES
                CXX_STANDARD 20
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                FOLDER "${TEST_TARGET_FOLDER}"
                VS_DEBUGGER_ENVIRONMENT "GTEST_CATCH_EXCEPTIONS=0"
        )

        # --- Sources
        if (DEFINED ARG_ABS_TESTS)
            target_sources(${ARG_TARGET_NAME}-test
                PRIVATE
                    ${ARG_ABS_TESTS}
            )
        endif()

        # --- Filters
        if (MSVC)
            # ARG_TESTS を見て, test/ を付加する
            if (DEFINED ARG_TESTS)
                foreach (rel_test_file IN LISTS ARG_TESTS)
                    # 絶対パス
                    file(REAL_PATH "test/${rel_test_file}" abs_test_file)

                    # フィルタ文字列
                    string(REPLACE "/" "\\" rel_test_file "${rel_test_file}")
                    cmake_path(GET rel_test_file PARENT_PATH rel_test_file)
                    source_group("${rel_test_file}" FILES "${abs_test_file}")
                    message(DEBUG "FILTER: ${rel_test_file} -> ${abs_test_file}")
                endforeach()
            endif()
        endif()

        # --- Test libraries ---
        target_link_libraries(${ARG_TARGET_NAME}-test
            PRIVATE
                GTest::gtest
                ${ARG_TARGET_NAME}
        )
        if (DEFINED ARG_LIBRARIES)
            target_link_libraries(${ARG_TARGET_NAME}-test
                PRIVATE
                    ${ARG_LIBRARIES}
            )
        endif()
        if (DEFINED ARG_TEST_LIBRARIES)
            target_link_libraries(${ARG_TARGET_NAME}-test
                PRIVATE
                    ${ARG_TEST_LIBRARIES}
            )
        endif()

        # --- compile_definitions ---
        target_compile_definitions(${ARG_TARGET_NAME}-test
            PRIVATE
                # Debug Mode Flag
                $<$<CONFIG:Debug>:ROAH_DEBUG=1>

                # Architecture Flags
                $<$<PLATFORM_ID:Windows>:ROAH_ARCH_WIN32>
                $<$<PLATFORM_ID:Linux>:ROAH_ARCH_LINUX>
        )
        if (DEFINED ARG_COMPILE_DEFINITIONS)
            target_compile_definitions(${ARG_TARGET_NAME}-test
                PRIVATE
                    ${ARG_COMPILE_DEFINITIONS}
                    ${ARG_PUBLIC_COMPILE_DEFINITIONS}
            )
        endif()

        # --- Resources ---
        if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/test/resource")
            file(
                COPY
                    "${CMAKE_CURRENT_LIST_DIR}/test/resource"
                DESTINATION
                    "${CMAKE_CURRENT_BINARY_DIR}")
            target_compile_definitions(${ARG_TARGET_NAME}-test
                PRIVATE
                    ROAH_TEST_RESOURCE_DIR="${CMAKE_CURRENT_BINARY_DIR}/resource"
            )
        endif()

        gtest_discover_tests(
            ${ARG_TARGET_NAME}-test
        )

    endif()
endfunction()


# =============================================================================================== #
#
# roah_add_executable
#
# - LIBRARIES (multi)
#   PRIVATE にリンクするライブラリ.
#
# - SOURCES (multi)
#   ソースファイル.
#
# - TESTS (multi)
#   テスト用のソースファイル.
#
# - COMPILE_DEFINITIONS (multi)
#   PRIVATE なコンパイル定数.
#
# - PUBLIC_COMPILE_DEFINITIONS (multi)
#   PUBLIC なコンパイル定数. 衝突に注意.
#
# - TEST_LIBRARIES (multi)
#   テスト用にのみリンクするライブラリ.
#
# - MSVC_SOURCES_FILTER_ROOT (single)
#   ソースファイルにおいて, この引数で指定したディレクトリ以下を MSVC のフィルタルートにします.
#
# - TARGET_FOLDER (single)
#   フィルタツリーにおいて, ライブラリを置く場所を指定する.
#
# =============================================================================================== #
function(roah_add_executable ARG_TARGET_NAME)
    set(options )
    set(oneValueArgs
        MSVC_SOURCES_FILTER_ROOT
        TARGET_FOLDER
    )
    set(multiValueArgs
        SOURCES
        TESTS
        LIBRARIES
        COMPILE_DEFINITIONS
        TEST_LIBRARIES
    )
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(TARGET_FOLDER "executables")
    if (DEFINED ARG_TARGET_FOLDER)
        set(TARGET_FOLDER "${ARG_TARGET_FOLDER}/executables")
    endif()

    set(TEST_TARGET_FOLDER "tests")
    if (DEFINED ARG_TARGET_FOLDER)
        set(TEST_TARGET_FOLDER "${ARG_TARGET_FOLDER}/tests")
    endif()

    if (DEFINED ARG_SOURCES)
        set(ARG_ABS_SOURCES )
        foreach (rel_source_file IN LISTS ARG_SOURCES)
            if (IS_ABSOLUTE "${rel_source_file}")
                message(SEND_ERROR "Absolute path is not supported. ${rel_source_file} (Target: ${ARG_TARGET_NAME})")
            else()
                file(REAL_PATH "src/${rel_source_file}" abs_source_file)
                list(APPEND ARG_ABS_SOURCES "${abs_source_file}")
                message(DEBUG "SOURCE: ${abs_source_file}")
            endif()
        endforeach()
    endif()

    # ARG_HEADERS を見て, include/ を付加する
    if (DEFINED ARG_HEADERS)
        set(ARG_ABS_HEADERS )
        foreach (rel_header_file IN LISTS ARG_HEADERS)
            if (IS_ABSOLUTE "${rel_header_file}")
                message(SEND_ERROR "Absolute path is not supported. ${rel_header_file} (Target: ${ARG_TARGET_NAME})")
            else()
                file(REAL_PATH "include/${rel_header_file}" abs_header_file)
                list(APPEND ARG_ABS_HEADERS "${abs_header_file}")
                message(DEBUG "HEADER: ${abs_header_file}")
            endif()
        endforeach()
    endif()

    add_executable(${ARG_TARGET_NAME})
    set_target_properties(${ARG_TARGET_NAME}
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            FOLDER "${TARGET_FOLDER}"
    )

    # --- Libraries ---
    if (DEFINED ARG_LIBRARIES)
        target_link_libraries(${ARG_TARGET_NAME}
            PRIVATE
                ${ARG_LIBRARIES}
        )
    endif()

    # --- compile definitions
    target_compile_definitions(${ARG_TARGET_NAME}
        PRIVATE
            # Debug Mode Flag
            $<$<CONFIG:Debug>:ROAH_DEBUG=1>

            # Architecture Flags
            $<$<PLATFORM_ID:Windows>:ROAH_ARCH_WIN32>
            $<$<PLATFORM_ID:Linux>:ROAH_ARCH_LINUX>
    )
    if (DEFINED ARG_COMPILE_DEFINITIONS)
        target_compile_definitions(${ARG_TARGET_NAME}
            PRIVATE
                ${ARG_COMPILE_DEFINITIONS}
        )
    endif()

    # --- Sources
    if (DEFINED ARG_ABS_SOURCES)
        target_sources(${ARG_TARGET_NAME}
            PRIVATE
                ${ARG_ABS_SOURCES}
        )
    endif()

    # --- filters
    if (MSVC)
        # ARG_SOURCES を見て, 相対パスである場合は src/ を付加する
        if (DEFINED ARG_SOURCES)
            if (DEFINED ARG_MSVC_SOURCES_FILTER_ROOT)
                string(REPLACE "/" "\\" sources_filter_root "${ARG_MSVC_SOURCES_FILTER_ROOT}")
            else()
                set(sources_filter_root "src")
            endif()

            foreach (rel_source_file IN LISTS ARG_SOURCES)
                # 絶対パス
                file(REAL_PATH "src/${rel_source_file}" abs_source_file)

                # フィルタ文字列
                string(REPLACE "/" "\\" rel_source_file "${rel_source_file}")
                string(REPLACE "${sources_filter_root}\\" "" rel_source_file "${rel_source_file}")
                cmake_path(GET rel_source_file PARENT_PATH rel_source_file)

                source_group("${rel_source_file}" FILES "${abs_source_file}")
                message(DEBUG "FILTER: ${rel_source_file} -> ${abs_source_file}")
            endforeach()
        endif()
    endif()

    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/resource")
        file(
            COPY
                "${CMAKE_CURRENT_LIST_DIR}/resource"
            DESTINATION
                "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    install(
        TARGETS
            ${ARG_TARGET_NAME}
        RUNTIME
    )

    # =======================================
    # test
    # =======================================
    if (BUILD_TESTS AND (DEFINED ARG_TESTS))

        # ARG_TESTS を見て, test/ を付加する
        if (DEFINED ARG_TESTS)
            set(ARG_ABS_TESTS )
            foreach (rel_test_file IN LISTS ARG_TESTS)
                if (IS_ABSOLUTE "${rel_test_file}")
                    message(SEND_ERROR "Absolute path is not supported. ${rel_test_file} (Target: ${ARG_TARGET_NAME})")
                else()
                    file(REAL_PATH "test/${rel_test_file}" abs_test_file)
                    list(APPEND ARG_ABS_TESTS "${abs_test_file}")
                    message(STATUS "TEST: ${abs_test_file}")
                endif()
            endforeach()
        endif()

        add_executable(${ARG_TARGET_NAME}-test)
        set_target_properties(${ARG_TARGET_NAME}-test
            PROPERTIES
                CXX_STANDARD 20
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                FOLDER "${TEST_TARGET_FOLDER}"
                VS_DEBUGGER_ENVIRONMENT "GTEST_CATCH_EXCEPTIONS=0"
        )

        # --- sources
        if (DEFINED ARG_ABS_TESTS)
            target_sources(${ARG_TARGET_NAME}-test
                PRIVATE
                    ${ARG_ABS_TESTS}
            )
        endif()

        # --- filters
        if (MSVC)
            # ARG_TESTS を見て, test/ を付加する
            if (DEFINED ARG_TESTS)
                foreach (rel_test_file IN LISTS ARG_TESTS)
                    # 絶対パス
                    file(REAL_PATH "test/${rel_test_file}" abs_test_file)

                    # フィルタ文字列
                    string(REPLACE "/" "\\" rel_test_file "${rel_test_file}")
                    cmake_path(GET rel_test_file PARENT_PATH rel_test_file)
                    source_group("${rel_test_file}" FILES "${abs_test_file}")
                    message(DEBUG "FILTER: ${rel_test_file} -> ${abs_test_file}")
                endforeach()
            endif()
        endif()

        # --- libs
        target_link_libraries(${ARG_TARGET_NAME}-test
            PRIVATE
                GTest::gtest
                ${ARG_TARGET_NAME}
        )
        if (DEFINED ARG_LIBRARIES)
            target_link_libraries(${ARG_TARGET_NAME}-test
                PRIVATE
                    ${ARG_LIBRARIES}
            )
        endif()
        if (DEFINED ARG_TEST_LIBRARIES)
            target_link_libraries(${ARG_TARGET_NAME}-test
                PRIVATE
                    ${ARG_TEST_LIBRARIES}
            )
        endif()

        # --- compile_definitions
        target_compile_definitions(${ARG_TARGET_NAME}-test
            PUBLIC
                # Debug Mode Flag
                $<$<CONFIG:Debug>:ROAH_DEBUG=1>

                # Architecture Flags
                $<$<PLATFORM_ID:Windows>:ROAH_ARCH_WIN32>
                $<$<PLATFORM_ID:Linux>:ROAH_ARCH_LINUX>
        )
        if (DEFINED ARG_COMPILE_DEFINITIONS)
            target_compile_definitions(${ARG_TARGET_NAME}-test
                PUBLIC
                    ${ARG_COMPILE_DEFINITIONS}
            )
        endif()

        if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/test/resource")
            file(
                COPY
                    "${CMAKE_CURRENT_LIST_DIR}/test/resource"
                DESTINATION
                    "${CMAKE_CURRENT_BINARY_DIR}")
        endif()

        gtest_discover_tests(
            ${ARG_TARGET_NAME}-test
        )

    endif()
endfunction()

function(export_config)
    install (
        EXPORT
            ${CMAKE_PROJECT_NAME}-export
        FILE
            ${CMAKE_PROJECT_NAME}Targets.cmake
        DESTINATION
            lib/cmake/${CMAKE_PROJECT_NAME}
        NAMESPACE
            ${CMAKE_PROJECT_NAME}::
        EXPORT_LINK_INTERFACE_LIBRARIES
    )

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
            "lib/cmake/${CMAKE_PROJECT_NAME}"
        NO_SET_AND_CHECK_MACRO
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
        VERSION
            "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}"
        COMPATIBILITY
            AnyNewerVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
            "lib/cmake/${CMAKE_PROJECT_NAME}"
    )

endfunction()
