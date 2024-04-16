cmake_minimum_required(VERSION 3.28)

project(
        hNVSE
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(external/common)

add_subdirectory(external/NVSE)

add_subdirectory(src)
