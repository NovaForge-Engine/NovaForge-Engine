cmake_minimum_required(VERSION 3.8.2) 
project(novaeditor)

STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(include)

file(GLOB_RECURSE SRCFILES 	
	RELATIVE ${PROJECT_SOURCE_DIR}
	./*.cpp;
	./*.c;
	./*.cxx;
	./*.h;
	./*.hpp;
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SRCFILES})


add_executable(${PROJECT_NAME} ${SRCFILES})

target_link_libraries(
	novaeditor PUBLIC
    novaengine
)

if (WIN32)
    add_custom_command(TARGET novaeditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:novaeditor> $<TARGET_FILE_DIR:novaeditor>
        COMMAND_EXPAND_LISTS
    )
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME}-bin)
install(FILES ${HEADER_FILES} DESTINATION ${PROJECT_NAME}-include)
