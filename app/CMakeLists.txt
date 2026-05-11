
add_subdirectory(deps)

roah_add_library(distb-utils
    LIBRARIES
        nlohmann_json::nlohmann_json

    SOURCES
        utils/subprocess.cpp
        utils/option_value.cpp
        utils/string_expander.cpp

    HEADERS
        roah/distb/utils/hash.hpp
        roah/distb/utils/option_value.hpp
        roah/distb/utils/string_expander.hpp
        roah/distb/utils/subprocess.hpp

    TESTS
        utils/hash.cpp
        utils/option_value.cpp
        utils/string_expander.cpp
        utils/utils_main.cpp

    MSVC_HEADERS_FILTER_ROOT
        roah/distb
)

# SHA-256 ハッシュ実装に必要なプラットフォーム固有のソースとライブラリをリンクする.
# Windows: BCrypt (Windows CNG API), Unix: OpenSSL libcrypto
if (WIN32)
    target_sources(distb-utils PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/hash_win32.cpp")
    target_link_libraries(distb-utils PRIVATE bcrypt)
else()
    find_package(OpenSSL REQUIRED)
    target_sources(distb-utils PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/hash_linux.cpp")
    target_link_libraries(distb-utils PRIVATE OpenSSL::Crypto)
endif()

roah_add_library(distb-confjg
    LIBRARIES
        nlohmann_json
        distb-utils
        
    SOURCES
        config/dependency_spec.cpp
        config/library_entry.cpp
        config/step_def.cpp

        config/impl/step_wget_impl.cpp
        config/impl/step_wget_impl.hpp

    HEADERS
        roah/distb/config/dependency_spec.hpp
        roah/distb/config/library_entry.hpp
        roah/distb/config/library.hpp
        roah/distb/config/option_value.hpp
        roah/distb/config/step_def.hpp

    MSVC_HEADERS_FILTER_ROOT
        roah/distb
)

roah_add_executable(distbuilder
    LIBRARIES
        CLI11::CLI11
        distb-confjg
        distb-utils

    SOURCES
        main.cpp
)