cmake_minimum_required(VERSION 4.2)
project(UmbraRT)

set(CMAKE_CXX_STANDARD 20)

add_executable(UmbraRT src/main.cpp
        include/Shader.h
        src/Shader.cpp
        include/Camera.h
        src/Camera.cpp
        include/Input.h
        src/Input.cpp
        include/Renderer.h
        src/Renderer.cpp
        include/Window.h
        src/Window.cpp
        include/DebugUI.h
        src/DebugUI.cpp
        include/Scene.h
        src/Scene.cpp
        include/Sphere.h
        include/AccumulationBuffer.h
        src/AccumulationBuffer.cpp
        include/Box.h
)

target_include_directories(UmbraRT PRIVATE
        include
        external/glm
        external/imgui
        external/imgui/backends
)

target_compile_definitions(UmbraRT PRIVATE GLFW_INCLUDE_NONE)

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