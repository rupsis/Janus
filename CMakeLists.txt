cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

project(Janus)

file(GLOB SOURCES
    main.cpp
    src/glad.c
    window/*.cpp
    tools/*.cpp
    opengl/*.cpp
    model/*.cpp
    imgui/*.cpp
    tinygltf/*.cc
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

add_executable(Janus ${SOURCES})

include_directories(${GLFW3_INCLUDE_DIR} include src window tools opengl model imgui tinygltf)

target_link_libraries(Janus PRIVATE glfw OpenGL::GL)