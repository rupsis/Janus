#pragma once
#include "OGLRenderData.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class VertexBuffer {
 public:
  void init();
  void uploadData(OGLMesh vertexData);
  void bind();
  void unbind();
  void draw(GLuint mode, unsigned int start, unsigned int num);
  void cleanup();

 private:
  GLuint mVAO = 0;
  GLuint mVertexVBO = 0;
};