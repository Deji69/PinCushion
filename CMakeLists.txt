cmake_minimum_required(VERSION 3.15)

project(PinCushion CXX)

# Find latest version at https://github.com/OrfeasZ/ZHMModSDK/releases
# Set ZHMMODSDK_DIR variable to a local directory to use a local copy of the ZHMModSDK.
set(ZHMMODSDK_VER "v3.1.1")
include(cmake/setup-zhmmodsdk.cmake)
include(cmake/get-cpm.cmake)

# Set C++ standard to C++23.
set(CMAKE_CXX_STANDARD 23)

CPMAddPackage("gh:OrfeasZ/ZHMTools@3.6.4")

# Create the PinCushion mod library.
add_library(PinCushion SHARED
    src/PinCushion.cpp
    src/PinCushion.h
    src/Properties.h
    src/Properties.cpp
)

# Set UTF-8 flag.
set_target_properties(PinCushion PROPERTIES COMPILE_FLAGS "/utf-8")

# Add SDK and DX headers dependencies.
find_package(directx-headers CONFIG REQUIRED)

target_link_libraries(PinCushion
    ZHMModSDK
	ResourceLib_HM3
    Microsoft::DirectX-Guids
    Microsoft::DirectX-Headers
)

install(TARGETS PinCushion
    RUNTIME DESTINATION bin
)

# Install the mod to the game folder when the `GAME_INSTALL_PATH` variable is set.
zhmmodsdk_install(PinCushion)
