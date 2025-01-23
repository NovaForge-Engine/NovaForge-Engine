cmake_minimum_required(VERSION 3.8.2) 
project(novaengine)


include_directories(${CMAKE_CURRENT_BINARY_DIR})


STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})


include_directories(include)

SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE SRCFILES 	
	RELATIVE ${PROJECT_SOURCE_DIR}
	./*.cpp;
	./*.c;
	./*.cxx;
	./*.h;
	./*.hpp;
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SRCFILES})



add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_library(novaengine SHARED ${SRCFILES})



target_link_libraries(
	novaengine 
	assimp
	spdlog
	glfw
	EnTT
	glm
)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_compile_definitions(FORGE_EXPORT)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME}-bin)
install(FILES ${HEADER_FILES} DESTINATION ${PROJECT_NAME}-include)

