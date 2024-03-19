cmake_minimum_required(VERSION 3.22)

set(APP jovial_gui)
set(JOVIAL /home/jove/Code/JovialEngine)

project(${APP})

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "-g")

add_definitions(
        -DJV_DEBUG
        -DJV_TARGET_LINUX
        -DJV_RENDERER_OPENGL
        -DJV_PHYSICS_DEBUG
)

add_library(pch INTERFACE)
target_precompile_headers(pch INTERFACE ${JOVIAL}/include/Jovial/pch.h)

set(JOVIAL_INCLUDES 
        ${JOVIAL}/include
        ${JOVIAL}/include/extern)

add_executable(${APP}
        src/main.cpp
        src/JovialGui.cpp
        src/Jimgui.cpp
)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

target_link_libraries(${APP} PRIVATE 
    ${JOVIAL}/build/libjovial_engine.a
    pch
    GL
    glfw)
target_include_directories(${APP} PUBLIC ${JOVIAL_INCLUDES})
