add_library("${PROJECT_NAME}" SHARED
        hnvse.cpp
        hnvse.hpp
        functions/hnvse_fn_region.hpp
        functions/hnvse_fn_worldspace.hpp
        hooks/hnvse_hk_region.hpp
        internal/hnvse_utility.cpp
        internal/hnvse_utility.hpp
)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:${PROJECT_NAME}> "$ENV{FalloutNVPath}/Data/NVSE/Plugins"
)

target_compile_options("${PROJECT_NAME}" PRIVATE
        /FI "${PROJECT_SOURCE_DIR}/external/NVSE/include/nvse/prefix.h"
)

target_include_directories("${PROJECT_NAME}" PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/external/common/include"
        "${PROJECT_SOURCE_DIR}/external/common/include/common"
        "${PROJECT_SOURCE_DIR}/external/NVSE/include"
        "${PROJECT_SOURCE_DIR}/external/NVSE/include/nvse"
)

target_link_libraries("${PROJECT_NAME}" PRIVATE
        common
        NVSE
)
