cmake_minimum_required(VERSION 4.2)
project(UmbraRT)

set(CMAKE_CXX_STANDARD 20)

add_executable(UmbraRT src/main.cpp
        include/Shader.h
        src/Shader.cpp)

add_subdirectory(external/glfw)

add_library(glad
        external/glad/src/gl.c
)

target_include_directories(glad PUBLIC
        external/glad/include
        include
)

target_link_libraries(UmbraRT
        glad
        glfw
)

find_package(OpenGL REQUIRED)

target_link_libraries(UmbraRT
        OpenGL::GL
)

file(COPY shaders
        DESTINATION ${CMAKE_BINARY_DIR})