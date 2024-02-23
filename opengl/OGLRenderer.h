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
  OGLRenderer(GLFWwindow *window);

  bool init(unsigned int width, unsigned int height);
  void setSize(unsigned int width, unsigned int height);
  void cleanup();
  void uploadData(OGLMesh vertexData);
  void draw();

  /* Key Handlers. */
  void handleKeyEvents(int key, int scancode, int action, int mods);

 private:
  Shader mBasicShader{};
  Shader mChangedShader{};
  Framebuffer mFramebuffer{};
  VertexBuffer mVertexBuffer{};
  Texture mTex{};
  GLFWwindow *mWindow = nullptr;
  int mTriangleCount = 0;
  bool mUseChangedShader = false;
};