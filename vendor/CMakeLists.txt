# vendor/NRI
add_subdirectory("NRI")
target_include_directories(NRI INTERFACE ${CMAKE_SOURCE_DIR}/vendor/NRI/Include)

# vendor/glfw
set (GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set (GLFW_BUILD_TESTS OFF CACHE BOOL "")
set (GLFW_BUILD_DOCS OFF CACHE BOOL "")
set (GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory ("glfw")
set_property (TARGET glfw PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
set_property (TARGET update_mappings PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
target_include_directories(glfw INTERFACE ${CMAKE_SOURCE_DIR}/vendor/glfw/include)

# vendor/detex
file (GLOB DETEX_SOURCE "detex/*.c" "detex/*.h")
source_group ("" FILES ${DETEX_SOURCE})
add_library (detex STATIC ${DETEX_SOURCE})
target_compile_definitions (detex PRIVATE ${COMPILE_DEFINITIONS})
set_property (TARGET detex PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
target_include_directories(detex INTERFACE ${CMAKE_SOURCE_DIR}/vendor/detex)

# vendor/Imgui
file (GLOB IMGUI_SOURCE "imgui/*.cpp")
file (GLOB IMGUI_HEADERS "imgui/*.h")
source_group ("" FILES ${IMGUI_SOURCE} ${IMGUI_HEADERS})
add_library (imgui STATIC ${IMGUI_SOURCE} ${IMGUI_HEADERS})
target_compile_definitions (imgui PRIVATE ${COMPILE_DEFINITIONS})
set_property (TARGET imgui PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
target_include_directories(imgui INTERFACE ${CMAKE_SOURCE_DIR}/vendor/imgui)

# vendor/assimp
set (ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory("assimp")
set_property (TARGET assimp PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
target_include_directories(assimp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendor/assimp/include>
    $<INSTALL_INTERFACE:assimp/include>
)

# vendor/spdlog
add_subdirectory("spdlog")
set_property(TARGET spdlog PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
target_include_directories(imgui INTERFACE ${CMAKE_SOURCE_DIR}/vendor/spdlog/include)

# vendor/entt
add_subdirectory("entt")
set_property(TARGET EnTT PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
target_include_directories(imgui INTERFACE ${CMAKE_SOURCE_DIR}/vendor/entt/src)

# vendor/glm
add_subdirectory("glm")
set_property (TARGET glm PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/vendor/glm)

# vendor/mono

# add libs based on what we have after build
# it is impied that you built mono in mono/build
# TODO: change this for the actual folder where mono is built on win32 with a VS solution
if(CMAKE_HOST_LINUX)
    add_library(monosgen SHARED IMPORTED GLOBAL)
    set_target_properties(monosgen PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/vendor/mono/build/lib/libmonosgen-2.0.so.1.0.0"
    )
    target_include_directories(monosgen INTERFACE "${CMAKE_SOURCE_DIR}/vendor/mono/build/include/mono-2.0")
elseif(CMAKE_HOST_WIN32)
    add_library(monosgen SHARED IMPORTED GLOBAL)
    set_target_properties(monosgen PROPERTIES
	    IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/vendor/mono/build/bin/mono-2.0-sgen.lib"
    )
    target_include_directories(monosgen INTERFACE "${CMAKE_SOURCE_DIR}/vendor/mono/build/include/")
endif()

# vendor/JoltPhysics
set(BUILD_SHARED_LIBS ON)
add_subdirectory("joltphysics/Build")
set_property (TARGET Jolt PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
set(BUILD_SHARED_LIBS OFF)

#vendor/magic_enum
add_subdirectory("magic_enum")
set_property (TARGET magic_enum PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
#target_include_directories(magic_enum INTERFACE ${CMAKE_SOURCE_DIR}/vendor/magic_enum)

#vendor/other
if(WIN32)
set_property (TARGET uninstall PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
set_property (TARGET UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
set_property (TARGET zlibstatic PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
#set_property (TARGET unit PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
endif()
