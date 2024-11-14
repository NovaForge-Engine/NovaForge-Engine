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
file (GLOB IMGUI_SOURCE "imgui/*.cpp" "imgui/*.h")
source_group ("" FILES ${IMGUI_SOURCE})
add_library (imgui STATIC ${IMGUI_SOURCE})
target_compile_definitions (imgui PRIVATE ${COMPILE_DEFINITIONS})
set_property (TARGET imgui PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")

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
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/vendor/mono/build/bin/mono-2.0-sgen.dll"
    )
    target_include_directories(monosgen INTERFACE "${CMAKE_SOURCE_DIR}/vendor/mono/build/include/mono-2.0")
endif()