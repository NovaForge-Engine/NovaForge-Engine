cmake_minimum_required(VERSION 3.8.2) 
project(NovaSandbox)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
 

STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

file(GLOB_RECURSE SRCFILES 	
	RELATIVE ${PROJECT_SOURCE_DIR}
	./*.cpp;
	./*.c;
	./*.cxx;
	./*.h;
	./*.hpp;
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SRCFILES})

set(DEBUG_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")



add_executable(NovaSandbox ${SRCFILES})
if (MSVC)
set_target_properties(
	NovaSandbox PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY ${DEBUG_WORKING_DIRECTORY}
)
endif()

set_target_properties(
	NovaSandbox PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/build/exec/release_with_debug_info"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/exec/release"
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}/build/exec/min_size_release"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build/exec/debug"
)



target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    NovaSandbox PUBLIC
    novaengine
    assimp
    imgui
    NRI
    detex
    spdlog
    glfw
    glm
	Jolt
    magic_enum
)

if (WIN32)
    add_custom_command(TARGET NovaSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:NovaSandbox> $<TARGET_FILE_DIR:NovaSandbox>
        COMMAND_EXPAND_LISTS
    )
endif()

install(TARGETS NovaSandbox DESTINATION NovaSandbox)
