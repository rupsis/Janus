#include "Shader.h"
#include "Logger.h"
#include <fstream>

bool Shader::loadShaders(std::string vertexShaderFileName, std::string fragmentShaderFileName) {
  GLuint vertexShader = readShader(vertexShaderFileName, GL_VERTEX_SHADER);
  if (!vertexShader) {
    Logger::log(1, "%s: Shader: Unable to read vertex shader\n", __FUNCTION__);
    return false;
  }

  GLuint fragmentShader = readShader(fragmentShaderFileName, GL_FRAGMENT_SHADER);
  if (!fragmentShader) {
    Logger::log(1, "%s: Shader: Unable to read fragment shader\n", __FUNCTION__);
    return false;
  }

  mShaderProgram = glCreateProgram();
  glAttachShader(mShaderProgram, vertexShader);
  glAttachShader(mShaderProgram, fragmentShader);
  // Link shader to GPU memory
  glLinkProgram(mShaderProgram);

  GLint isProgramLinked;
  glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &isProgramLinked);

  if (!isProgramLinked) {
    Logger::log(1, "%s: Shader: Error linking shader program\n", __FUNCTION__);
    return false;
  }

  // Cleanup
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return true;
}

void Shader::cleanup() {
  GLuint uboIndex = glGetUniformBlockIndex(mShaderProgram, "Matrices");
  glUniformBlockBinding(mShaderProgram, uboIndex, 0);
  glDeleteProgram(mShaderProgram);
}

GLuint Shader::readShader(std::string shaderFileName, GLuint shaderType) {
  // GLuint shader;
  std::string shaderAsText;
  std::ifstream inFile(shaderFileName);

  if (inFile.is_open()) {
    inFile.seekg(0, std::ios::end);
    shaderAsText.reserve(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    // Read contents of shader into string
    shaderAsText.assign((std::istreambuf_iterator<char>(inFile)),
                        std::istreambuf_iterator<char>());
    // Close file after reading.
    inFile.close();
  }
  else {
    Logger::log(1, "%s: Shader: Error, unable to read shader file\n", __FUNCTION__);
    return 0;
  }

  if (inFile.bad() || inFile.fail()) {
    inFile.close();
    Logger::log(1, "%s: Shader: Error, unable to read shader file\n", __FUNCTION__);
    return 0;
  }

  inFile.close();
  const char *shaderSource = shaderAsText.c_str();

  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, (const GLchar **)&shaderSource, 0);
  glCompileShader(shader);

  GLint isShaderCompiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);
  if (!isShaderCompiled) {
    Logger::log(1, "%s: Shader: Error, unable to compile shader\n", __FUNCTION__);
    return 0;
  }

  // If shader is compiled correctly, return handle
  return shader;
}

void Shader::use() {
  glUseProgram(mShaderProgram);
}

/* There is no "unuse"  binding, because there always needs
 * to be an active shader to avoid undefined behavior.
 */