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

#include "SharedStorageBuffer.h"
#include "UniformBuffer.h"
#include "UserInterface.h"
#include "VertexBuffer.h"

/* Tools */
#include "Camera.h"
#include "Timer.h"

#include "GltfModel.h"

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

  /* Mouse Handlers. */
  void handleMouseButtonEvents(int button, int action, int mods);
  void handleMousePositionEvents(double xPos, double yPos);

 private:
  void handleMovementKeys();

  OGLRenderData mRenderData{};
  UserInterface mUserInterface{};

  /* Model. */
  Shader mGltfShader{};
  Shader mGltfGPUShader{};
  std::shared_ptr<GltfModel> mGltfModel = nullptr;
  bool mModelUploadRequired = true;

  /* Shaders. */
  Shader mBasicShader{};
  Shader mChangedShader{};

  /* Texture. */
  Texture mTex{};

  /* Buffers. */
  Framebuffer mFramebuffer{};
  VertexBuffer mVertexBuffer{};
  UniformBuffer mUniformBuffer{};
  SharedStorageBuffer mGltfShaderStorageBuffer{};

  /* UniformBuffer Data. */
  glm::mat4 mViewMatrix = glm::mat4(1.0f);
  glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

  Timer mUIGenerateTimer{};
  Camera mCamera{};

  /* Mouse values. */
  bool mMouseLock = false;
  int mMouseXPos = 0;
  int mMouseYPos = 0;

  // Difference between current & previous draw() calls
  double lastTickTime = 0.0;
};