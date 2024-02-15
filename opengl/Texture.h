#pragma once
#include <glad/glad.h>
#include <string>

#include <GLFW/glfw3.h>

class Texture {
 public:
  bool loadTexture(std::string textureFilename);
  void bind();
  void unbind();
  void cleanup();

 private:
  GLuint mTexture = 0;
};