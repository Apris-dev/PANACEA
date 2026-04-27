# CMake Parent File Requirements
cmake_minimum_required(VERSION 3.31...4.2)

set(CMAKE_CXX_STANDARD 23)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Project Definition

include(third_party/simplecpp/scripts/setup_vcpkg.cmake)

# Ensures vcpkg toolchain
setup_vcpkg()

project(
    PANACEA
    VERSION 1.0.0
    LANGUAGES CXX C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(BUILD_DIR PROJECT_BINARY_DIR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Code

add_subdirectory(third_party)

option(SIMPLEPTR_INIT "" ON)
option(SIMPLEPTR_DESTROY "" ON)
option(SIMPLEPTR_RELEASE "" ON)
add_subdirectory(third_party/simplecpp)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(directx-dxc CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(sdl3 CONFIG REQUIRED)
find_package(vk-bootstrap CONFIG REQUIRED)
find_package(fastgltf CONFIG REQUIRED)
find_package(meshoptimizer CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(basisu CONFIG REQUIRED)
find_package(tracy CONFIG REQUIRED)

add_subdirectory(src)