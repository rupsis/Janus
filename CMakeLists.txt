cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

project(Janus)

file(GLOB SOURCES
    main.cpp
    window/*.cpp
    tools/*.cpp
    src/glad.c
    model/*.cpp
    opengl/*.cpp
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

add_executable(Janus ${SOURCES})

include_directories(${GLFW3_INCLUDE_DIR} window tools src include model opengl)

target_link_libraries(Janus PRIVATE glfw OpenGL::GL)