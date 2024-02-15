#pragma once

// Include glad.h before GLFW,
// as it changes it's behavior
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Framebuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"

#include "OGLRenderData.h"

class OGLRenderer {
 public:
  bool init(unsigned int width, unsigned int height);
  void setSize(unsigned int width, unsigned int height);
  void cleanup();
  void uploadData(OGLMesh vertexData);
  void draw();

 private:
  Shader mShader{};
  Framebuffer mFramebuffer{};
  VertexBuffer mVertexBuffer{};
  Texture mTex{};
  int mTriangleCount = 0;
};