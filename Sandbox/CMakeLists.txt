cmake_minimum_required(VERSION 3.8.2) 
project(NovaSandbox)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# elizoorg 25.09.2024
# TODO: We can setup all project files as glob
set(HEADER_FILES 

)



set(MODULE_FILES
    src/main.cpp
    src/Utils.hpp
    src/Utils.cpp
    src/NRIContext.h
    src/IRenderPass.h
    src/IRenderPass.cpp
    src/ISystem.h
    src/ISystem.cpp
    src/PassCompositor.h
    src/PassCompositor.cpp
    src/UIRenderPass.h
    src/UIRenderPass.cpp
    src/MainRenderPass.h
    src/MainRenderPass.cpp
    src/RenderSystem.h
    src/RenderSystem.cpp
    src/Window.h
    src/Window.cpp
    src/Packing.h
    src/Packing.cpp
)

set(INTERFACE_FILES
    src/IRenderPass.h
    src/IRenderPass.cpp
    src/ISystem.h
    src/ISystem.cpp
)

source_group("Interface" FILES ${INTERFACE_FILES})

set(SYSTEM_FILES
    src/RenderSystem.h
    src/RenderSystem.cpp
)

source_group("Systems" FILES ${SYSTEM_FILES})

set(RENDER_FILES
    src/PassCompositor.h
    src/PassCompositor.cpp
    src/UIRenderPass.h
    src/UIRenderPass.cpp
    src/MainRenderPass.h
    src/MainRenderPass.cpp
)

source_group("Render" FILES ${RENDER_FILES})


add_executable(NovaSandbox ${MODULE_FILES} ${HEADER_FILES})



target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/NRI/Include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/detex)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/MathLib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/assimp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glm)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui)

target_link_libraries(
    NovaSandbox PUBLIC
    novaengine
    assimp
    imgui
    NRI
    detex
    glfw
)








if (WIN32)
    add_custom_command(TARGET NovaSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:NovaSandbox> $<TARGET_FILE_DIR:NovaSandbox>
        COMMAND_EXPAND_LISTS
    )
endif()

install(TARGETS NovaSandbox DESTINATION NovaSandbox)
