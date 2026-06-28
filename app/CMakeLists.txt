
roah_add_library(distb-core
    SOURCES
        errors.cpp
        logger.cpp
        working_context.cpp

    HEADERS
        roah/distb/errors.hpp
        roah/distb/logger.hpp
        roah/distb/working_context.hpp

    MSVC_HEADERS_FILTER_ROOT
        roah/distb
)

roah_add_library(distb-utils
    LIBRARIES
        nlohmann_json::nlohmann_json
        distb-config

    SOURCES
        utils/base32.cpp
        utils/hash.cpp
        utils/subprocess.cpp
        utils/option_value.cpp
        utils/path.cpp
        utils/string_expander.cpp
        utils/string.cpp

    HEADERS
        roah/distb/utils/base32.hpp
        roah/distb/utils/delay_copyable_container.hpp
        roah/distb/utils/hash.hpp
        roah/distb/utils/option_value.hpp
        roah/distb/utils/path.hpp
        roah/distb/utils/string_expander.hpp
        roah/distb/utils/string.hpp
        roah/distb/utils/subprocess.hpp

    TESTS
        utils/base32.cpp
        utils/hash.cpp
        utils/option_value.cpp
        utils/string_expander.cpp
        utils/string.cpp
        utils/utils_main.cpp

    MSVC_HEADERS_FILTER_ROOT
        roah/distb
)

# SHA-256 ハッシュ実装に必要なプラットフォーム固有のソースとライブラリをリンクする.
# Windows: BCrypt (Windows CNG API), Unix: OpenSSL libcrypto
if (WIN32)
    target_sources(distb-utils
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/impl/hash_win32.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/impl/hash_win32.hpp"
    )
    target_link_libraries(distb-utils PRIVATE bcrypt)
else()
    find_package(OpenSSL REQUIRED)
    target_sources(distb-utils
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/impl/hash_unix.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/impl/hash_unix.hpp"
    )
    target_link_libraries(distb-utils PRIVATE OpenSSL::Crypto)
endif()

roah_add_library(distb-config
    LIBRARIES
        nlohmann_json::nlohmann_json
        distb-core
        distb-utils
        
    SOURCES
        config/condition.cpp
        config/dependency_spec.cpp
        config/library_entry.cpp
        config/library.cpp
        config/step_def.cpp

        config/impl/condition_and_op_impl.cpp
        config/impl/condition_and_op_impl.hpp
        config/impl/condition_and_or_op_impl.cpp
        config/impl/condition_and_or_op_impl.hpp
        config/impl/condition_is_false_op_impl.cpp
        config/impl/condition_is_false_op_impl.hpp
        config/impl/condition_is_op_impl.cpp
        config/impl/condition_is_op_impl.hpp
        config/impl/condition_is_true_op_impl.cpp
        config/impl/condition_is_true_op_impl.hpp
        config/impl/condition_or_op_impl.cpp
        config/impl/condition_or_op_impl.hpp

        config/impl/step_cmake_configure_impl.cpp
        config/impl/step_cmake_configure_impl.hpp
        config/impl/step_cmake_install_all_impl.cpp
        config/impl/step_cmake_install_all_impl.hpp
        config/impl/step_download_impl.cpp
        config/impl/step_download_impl.hpp
        config/impl/step_extract_impl.cpp
        config/impl/step_extract_impl.hpp
        config/impl/step_install_file_impl.cpp
        config/impl/step_install_file_impl.hpp

    HEADERS
        roah/distb/config/condition.hpp
        roah/distb/config/dependency_spec.hpp
        roah/distb/config/library_entry.hpp
        roah/distb/config/library.hpp
        roah/distb/config/step_def.hpp
        roah/distb/config/variables.hpp

    MSVC_HEADERS_FILTER_ROOT
        roah/distb
)

roah_add_executable(distbuilder
    LIBRARIES
        CLI11::CLI11
        toml11::toml11
        nlohmann_json::nlohmann_json
        distb-config
        distb-utils

    SOURCES
        main.cpp

        app/app_config_platform.cpp
        app/app_config.cpp
        app/app_config.hpp
        app/app.cpp
        app/app.hpp
        app/dependency.cpp
        app/dependency.hpp
        app/library_store.cpp
        app/library_store.hpp
        app/working_context_impl.cpp
        app/working_context_impl.hpp
)

target_compile_definitions(distbuilder
    PRIVATE
        $<$<PLATFORM_ID:Windows>:ROAH_ARCH_WIN32=1>
        $<$<PLATFORM_ID:MacOS>:ROAH_ARCH_MACOS=1>
        $<$<PLATFORM_ID:Linux>:ROAH_ARCH_LINUX=1>
        DISTBUILDER3_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        DISTBUILDER3_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        DISTBUILDER3_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

set(ROAH_DISTB_CONF_PATH        "${CMAKE_CURRENT_BINARY_DIR}/distbuilder.conf")
set(ROAH_DISTB_CONF_SEARCH_PATH "\"S:/works/programming/distbuilder3-libraries/libs\"")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/distbuilder.in.conf"
    "${ROAH_DISTB_CONF_PATH}"
    @ONLY
)

set_target_properties(distbuilder
    PROPERTIES
        DEBUG_POSTFIX
            "_d"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CMAKE_SOURCE_DIR}/test_project"
        VS_DEBUGGER_COMMAND_ARGUMENTS
            "--verbose --export-presets --gather-licenses licenses ."
)