# vendor/NRI
add_subdirectory("NRI")

# vendor/glfw
option (GLFW_BUILD_EXAMPLES "" OFF)
option (GLFW_BUILD_TESTS "" OFF)
option (GLFW_BUILD_DOCS "" OFF)
option (GLFW_INSTALL "" OFF)
add_subdirectory ("glfw")
set_property (TARGET glfw PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
set_property (TARGET update_mappings PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")


# vendor/detex
file (GLOB DETEX_SOURCE "detex/*.c" "detex/*.h")
source_group ("" FILES ${DETEX_SOURCE})
add_library (detex STATIC ${DETEX_SOURCE})
target_compile_definitions (detex PRIVATE ${COMPILE_DEFINITIONS})
set_property (TARGET detex PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")

# vendor/Imgui
file (GLOB IMGUI_SOURCE "imgui/*.cpp" "imgui/*.h")
source_group ("" FILES ${IMGUI_SOURCE})
add_library (imgui STATIC ${IMGUI_SOURCE})
target_compile_definitions (imgui PRIVATE ${COMPILE_DEFINITIONS})
set_property (TARGET imgui PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")

# vendor/assimp
add_subdirectory("assimp")
set_property (TARGET assimp PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")


add_subdirectory("glm")
set_property (TARGET glm PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")