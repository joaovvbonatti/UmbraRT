cmake_minimum_required(VERSION 4.2)
project(UmbraRT)

set(CMAKE_CXX_STANDARD 20)

add_executable(UmbraRT src/main.cpp
        include/Shader.h
        src/Shader.cpp
        include/Camera.h
        src/Camera.cpp
        include/Input.h
        src/Input.cpp)

target_include_directories(UmbraRT PRIVATE
        include
        external/glm
        external/imgui
        external/imgui/backends
)

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

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
        COMMENT "Copying shaders to build directory"
)

add_dependencies(UmbraRT copy_shaders)

target_sources(UmbraRT PRIVATE

        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp

        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)