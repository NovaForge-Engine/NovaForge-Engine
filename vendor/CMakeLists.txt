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
set_property (TARGET zlibstatic PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
set_property (TARGET uninstall PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
set_property (TARGET UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
if(BUILD_STATIC_LIBS)
	set_property (TARGET unit PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
endif()
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/vendor/glm)

# vendor/JoltPhysics
add_subdirectory("joltphysics/Build")
target_include_directories(Jolt INTERFACE ${CMAKE_SOURCE_DIR}/vendor/joltphysics)
set_property (TARGET Jolt PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
