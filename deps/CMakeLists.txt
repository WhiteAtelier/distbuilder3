
include(FetchContent)
set(CMAKE_FOLDER       "deps")
set(CMAKE_CXX_STANDARD 20)

# -----------------------------------------------------------------------------
# jbeder/yaml-cpp
# -----------------------------------------------------------------------------
if (OFF)
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        yaml-cpp-0.9.0
    GIT_SHALLOW
        TRUE
        OVERRIDE_FIND_PACKAGE
)

# yaml-cpp のテストや util は不要なので無効化.
set(YAML_CPP_BUILD_TESTS   OFF)
set(YAML_CPP_BUILD_TOOLS   OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)

FetchContent_MakeAvailable(yaml-cpp)
endif()

# -----------------------------------------------------------------------------
# nlohmann/json
# -----------------------------------------------------------------------------
find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)

    set(JSON_BuildTests          OFF)
    set(JSON_CI                  OFF)
    set(JSON_Diagnostics         OFF)
    set(JSON_GlobalUDLs          OFF)
    set(JSON_ImplicitConversions ON)
    set(JSON_MultipleHeaders     ON)
    set(JSON_Install             OFF)

    FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY
            https://github.com/nlohmann/json
        GIT_TAG
            55f93686c01528224f448c19128836e7df245f72  # v3.12
        GIT_SHALLOW
            TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(nlohmann_json)

endif()


# -----------------------------------------------------------------------------
# ToruNiina/toml11
# -----------------------------------------------------------------------------
find_package(toml11 QUIET)
if (NOT toml11_FOUND)
    set(TOML11_PRECOMPILE          ON)
    set(TOML11_ENABLE_ACCESS_CHECK OFF)
    set(TOML11_INSTALL             OFF)
    set(TOML11_BUILD_EXAMPLES      OFF)
    set(TOML11_BUILD_TESTS         OFF)
    set(TOML11_BUILD_TOML_TESTS    OFF)
    set(TOML11_TEST_WITH_ASAN      OFF)
    set(TOML11_TEST_WITH_UBSAN     OFF)

    FetchContent_Declare(toml11
        GIT_REPOSITORY
            https://github.com/ToruNiina/toml11
        GIT_TAG
            be08ba2be2a964edcdb3d3e3ea8d100abc26f286  # v4.4.0
        GIT_SHALLOW
            TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(toml11)

endif()

# -----------------------------------------------------------------------------
# CLIUtils/CLI11
# -----------------------------------------------------------------------------
find_package(CLI11 QUIET)
if (NOT CLI11_FOUND)

    set(CLI11_BUILD_TESTS         OFF)
    set(CLI11_BUILD_EXAMPLES      OFF)
    set(CLI11_BUILD_DOCS          OFF)
    set(CLI11_SINGLE_FILE         OFF)
    set(CLI11_PRECOMPILED         ON)
    set(CLI11_MODULES             OFF)
    set(CLI11_SANITIZERS          OFF)
    set(CLI11_BUILD_EXAMPLES_JSON OFF)
    set(CLI11_FULL_INSTALL        OFF)
    set(CLI11_SINGLE_FILE_TESTS   OFF)
    set(CLI11_INSTALL             OFF)
    set(CLI11_FORCE_LIBCXX        OFF)

    FetchContent_Declare(CLI11
        GIT_REPOSITORY
            https://github.com/CLIUtils/CLI11
        GIT_TAG
            37bb6edc5317e99af72ef48405e65d9ca5218861  # v2.6.2
        GIT_SHALLOW
            TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(CLI11)

endif()

# -----------------------------------------------------------------------------
# google/googletest
# -----------------------------------------------------------------------------
if (DISTBUILDER3_BUILD_TESTS)
    find_package(GTest QUIET)
    if (NOT GTest_FOUND)

        set(INSTALL_GTEST     OFF)
        set(BUILD_GMOCK       OFF)
        set(BUILD_SHARED_LIBS OFF)

        FetchContent_Declare(GTest
            GIT_REPOSITORY
                https://github.com/google/googletest
            GIT_TAG
                52eb8108c5bdec04579160ae17225d66034bd723  # v1.17.0
            GIT_SHALLOW
                TRUE
            OVERRIDE_FIND_PACKAGE
        )
        FetchContent_MakeAvailable(GTest)

    endif()
endif()
