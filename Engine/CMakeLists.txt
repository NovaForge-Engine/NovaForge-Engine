cmake_minimum_required(VERSION 3.8.2) 
project(novaengine)


include_directories(${CMAKE_CURRENT_BINARY_DIR})


STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})


include_directories(include)

SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE HEADER_FILES COMPILE_DEPENDS
	include/*.h include/*.hpp
)



file(GLOB MODULE_FILES COMPILE_DEPENDS
	src/*.cpp
)


add_library(novaengine SHARED ${MODULE_FILES} ${HEADER_FILES})



target_link_libraries(
	novaengine 
	assimp
	imgui
	glfw
	EnTT
	glm
)


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_compile_definitions(FORGE_EXPORT)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME}-bin)
install(FILES ${HEADER_FILES} DESTINATION ${PROJECT_NAME}-include)

