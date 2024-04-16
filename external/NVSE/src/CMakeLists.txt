add_library("${PROJECT_NAME}" STATIC
        "containers.cpp"
        "GameAPI.cpp"
        "GameBSExtraData.cpp"
        "GameData.cpp"
        "GameExtraData.cpp"
        "GameForms.cpp"
        "GameObjects.cpp"
        "GameOSDepend.cpp"
        "GameProcess.cpp"
        "GameRTTI.cpp"
        "GameScript.cpp"
        "GameSettings.cpp"
        "GameTasks.cpp"
        "GameTiles.cpp"
        "GameTypes.cpp"
        "NiTypes.cpp"
        "printf.cpp"
        "SafeWrite.cpp"
        "Utilities.cpp"
        "utility.cpp"
)

target_compile_definitions("${PROJECT_NAME}" PUBLIC WIN32 NDEBUG _WINDOWS _USRDLL RUNTIME RUNTIME_VERSION=0x040020D0)

target_compile_options("${PROJECT_NAME}" PRIVATE
        /FI "${PROJECT_SOURCE_DIR}/include/nvse/prefix.h"
)

target_include_directories("${PROJECT_NAME}" PRIVATE
        "${CMAKE_SOURCE_DIR}/external/common/include"
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/include/nvse"
)

target_link_libraries("${PROJECT_NAME}" PRIVATE common)
