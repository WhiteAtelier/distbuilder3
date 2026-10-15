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

# OS はバージョンやディストリビューションを含めず, 小文字で表記する.
string(TOLOWER "${CMAKE_SYSTEM_NAME}" distbuilder_package_platform)

set(CPACK_PACKAGE_FILE_NAME
    "${PROJECT_NAME}-${PROJECT_VERSION}-${distbuilder_package_platform}-${distbuilder_package_arch}"
)

include(CPack)
