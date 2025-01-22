cmake_minimum_required(VERSION 3.27)
# Enable Hot Reload for MSVC compilers if supported.
project(novaforge)


include(CMakePrintHelpers)

set(CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (MSVC)
  add_compile_options(/bigobj)
#else ()
  #add_compile_options(-Wa,-mbig-obj)
endif ()

if(MSVC)
  add_definitions(/MP)
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Установка 20-го стандарта
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CPP_RTTI_ENABLED ON)

# Include sub-projects.
add_subdirectory("vendor")

add_subdirectory("Engine")
add_subdirectory("Editor")
add_subdirectory("Sandbox")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NovaSandbox)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

if (NOT TARGET ShaderMake)
    set (SHADERMAKE_BIN_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} CACHE STRING "")
    add_subdirectory (vendor/ShaderMake)
    set_property (TARGET ShaderMake PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
    set_property (TARGET ShaderMakeBlob PROPERTY FOLDER "${PROJECT_FOLDER}/vendor")
endif ()

if ("${NRIF_SHADERS_PATH}" STREQUAL "")
    set (NRIF_SHADERS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/_Shaders")
endif ()

message ("NRI framework shaders output path: '${NRIF_SHADERS_PATH}'")

file (GLOB_RECURSE SHADERS "_Data/Shaders/*.hlsl")
set_source_files_properties (${SHADERS} PROPERTIES VS_TOOL_OVERRIDE "None")


set (SHADERMAKE_GENERAL_ARGS
    --useAPI --binary --flatten --stripReflection --WX
    --sourceDir "_Data/Shaders"
    -c Shaders.cfg
    -o "${NRIF_SHADERS_PATH}"
    -I "vendor/NRI/Include"
    -I "vendor/ml"
    --sRegShift 100 --tRegShift 200 --bRegShift 300 --uRegShift 400
)

if (WIN32)
    add_custom_target (${PROJECT_NAME}_Shaders ALL
        COMMAND ShaderMake ${SHADERMAKE_GENERAL_ARGS} -p DXIL --compiler "${DXC_PATH}"
        COMMAND ShaderMake ${SHADERMAKE_GENERAL_ARGS} -p SPIRV --compiler "${DXC_SPIRV_PATH}"
        DEPENDS ShaderMake
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        VERBATIM
        SOURCES ${SHADERS}
    )
else ()
    add_custom_target (${PROJECT_NAME}_Shaders ALL
        COMMAND ShaderMake ${SHADERMAKE_GENERAL_ARGS} -p SPIRV --compiler "${DXC_SPIRV_PATH}"
        DEPENDS ShaderMake
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        VERBATIM
        SOURCES ${SHADERS}
    )
endif ()






set(NOVA_FORGE_VERSION_MAJOR 0)
set(NOVA_FORGE_VERSION_MINOR 0)
set(NOVA_FORGE_VERSION_PATCH 1)


include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_NOVA_FORGE_VERSION_MAJOR ${NOVA_FORGE_VERSION_MAJOR})
set(CPACK_NOVA_FORGE_VERSION_MINOR ${NOVA_FORGE_VERSION_MINOR})
set(CPACK_NOVA_FORGE_VERSION_PATCH ${NOVA_FORGE_VERSION_PATCH})
include(CPack)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)
endif()
