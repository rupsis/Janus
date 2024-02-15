#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <string>

class Shader {
 public:
  bool loadShaders(std::string vertexShaderFileName, std::string fragmentShaderFileName);
  void use();
  void cleanup();

 private:
  GLuint mShaderProgram = 0;
  GLuint readShader(std::string shaderFileName, GLuint shaderType);
};