cmake_minimum_required(VERSION 3.8.2) 
project(novaeditor)

STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(include)

# elizoorg 25.09.2024
# TODO: We can setup all project files as glob
set(HEADER_FILES 

)

set(MODULE_FILES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${MODULE_FILES} ${HEADER_FILES})

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
