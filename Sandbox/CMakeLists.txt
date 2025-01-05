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





add_executable(NovaSandbox ${SRCFILES})



target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    NovaSandbox PUBLIC
    novaengine
    assimp
    imgui
    NRI
    detex
    glfw
    glm
)

if (WIN32)
    add_custom_command(TARGET NovaSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:NovaSandbox> $<TARGET_FILE_DIR:NovaSandbox>
        COMMAND_EXPAND_LISTS
    )
endif()

install(TARGETS NovaSandbox DESTINATION NovaSandbox)
