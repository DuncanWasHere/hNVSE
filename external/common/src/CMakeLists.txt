file(GLOB COMMON_SOURCES *.cpp)

add_library("${PROJECT_NAME}" STATIC
        "${COMMON_SOURCES}"
)

target_compile_definitions("${PROJECT_NAME}" PUBLIC WIN32 NDEBUG _WINDOWS _USRDLL RUNTIME RUNTIME_VERSION=0x040020D0)

target_compile_options("${PROJECT_NAME}" PRIVATE
        /FI "${CMAKE_SOURCE_DIR}/external/NVSE/include/nvse/prefix.h"
)

target_include_directories("${PROJECT_NAME}" PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/include/common"
        "${CMAKE_SOURCE_DIR}/external/NVSE/include"
        "${CMAKE_SOURCE_DIR}/external/NVSE/include/nvse"
)

