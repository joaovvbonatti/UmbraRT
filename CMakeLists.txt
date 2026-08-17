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
        include/Material.h
        include/Plane.h
        src/Model/Vertex.h
        src/Model/Mesh.h
        src/Model/Model.h
        src/Model/ModelLoader.h
        src/Model/ModelLoader.cpp
        src/Model/Triangle.h
        include/TriangleBuffer.h
        src/TriangleBuffer.cpp
)

target_include_directories(UmbraRT PRIVATE
        include
        src
        external/glm
        external/imgui
        external/imgui/backends
        external/tinyobjloader
)

target_compile_definitions(UmbraRT PRIVATE GLFW_INCLUDE_NONE)

add_subdirectory(external/glfw)

add_library(glad
        external/glad/src/gl.c
)

target_include_directories(glad PUBLIC
        external/glad/include
)

target_link_libraries(UmbraRT
        glad
        glfw
)

find_package(OpenGL REQUIRED)

target_link_libraries(UmbraRT
        OpenGL::GL
)

add_custom_command(TARGET UmbraRT POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:UmbraRT>/shaders

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:UmbraRT>/assets
)

target_sources(UmbraRT PRIVATE

        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp

        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)