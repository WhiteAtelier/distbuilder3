# This file contains code generated with the assistance of Codex (OpenAI), an AI assistant.
# The generated code is provided as-is.

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DIRECTORY "${DISTBUILDER3_CPACK_PACKAGE_DIRECTORY}")
set(CPACK_GENERATOR "TGZ")

# MSVC ではホストではなく, ビルド対象の architecture を使用する.
set(distbuilder_package_arch "${CMAKE_SYSTEM_PROCESSOR}")
if (MSVC AND MSVC_CXX_ARCHITECTURE_ID)
    set(distbuilder_package_arch "${MSVC_CXX_ARCHITECTURE_ID}")
endif()

string(TOLOWER "${distbuilder_package_arch}" distbuilder_package_arch)
if (distbuilder_package_arch MATCHES "^(amd64|x64|x86_64)$")
    set(distbuilder_package_arch "x86_64")
elseif (distbuilder_package_arch MATCHES "^(arm64|aarch64)$")
    set(distbuilder_package_arch "aarch64")
elseif (distbuilder_package_arch MATCHES "^(x86|i[3-6]86)$")
    set(distbuilder_package_arch "x86")
endif()

# ネイティブビルドでは, ビルド環境の OS バージョンを含める.
set(distbuilder_package_platform "${CMAKE_SYSTEM_NAME}")
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_CROSSCOMPILING)
    cmake_host_system_information(RESULT distbuilder_distrib QUERY DISTRIB_INFO)
    if (distbuilder_distrib_NAME AND distbuilder_distrib_VERSION_ID)
        string(REGEX REPLACE "[^A-Za-z0-9._-]+" "-" distbuilder_package_platform
            "${distbuilder_distrib_NAME}-${distbuilder_distrib_VERSION_ID}"
        )
    endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND NOT CMAKE_CROSSCOMPILING)
    if (CMAKE_HOST_SYSTEM_VERSION)
        set(distbuilder_package_platform "Windows-${CMAKE_HOST_SYSTEM_VERSION}")
    endif()
endif()

set(CPACK_PACKAGE_FILE_NAME
    "${PROJECT_NAME}-${PROJECT_VERSION}-${distbuilder_package_platform}-${distbuilder_package_arch}"
)

include(CPack)
