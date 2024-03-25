#pragma once

// Include glad.h before GLFW,
// as it changes it's behavior
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Camera.h"
#include "Framebuffer.h"
#include "GltfModel.h"
#include "Shader.h"
#include "ShaderStorageBuffer.h"
#include "Texture.h"
#include "Timer.h"
#include "UniformBuffer.h"
#include "UserInterface.h"
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

  /* Mouse Handlers. */
  void handleMouseButtonEvents(int button, int action, int mods);
  void handleMousePositionEvents(double xPos, double yPos);

 private:
  void handleMovementKeys();

  OGLRenderData mRenderData{};
  UserInterface mUserInterface{};

  /* Model. */
  std::shared_ptr<GltfModel> mGltfModel = nullptr;
  bool mModelUploadRequired = true;

  std::shared_ptr<OGLMesh> mSkeletonMesh = nullptr;
  unsigned int mSkeletonLineIndexCount = 0;

  /* Shaders. */
  Shader mGltfShader{};
  Shader mGltfGPUShader{};
  Shader mGltfGPUDualQuatShader{};

  Shader mLineShader{};
  Shader mBasicShader{};
  Shader mChangedShader{};

  /* Texture. */
  Texture mTex{};

  /* Buffers. */
  Framebuffer mFramebuffer{};
  VertexBuffer mVertexBuffer{};
  UniformBuffer mUniformBuffer{};
  ShaderStorageBuffer mGltfShaderStorageBuffer{};
  ShaderStorageBuffer mGltfDualQuatSSBuffer{};

  /* UniformBuffer Data. */
  glm::mat4 mViewMatrix = glm::mat4(1.0f);
  glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

  /* Timers*/
  Timer mFrameTimer{};
  Timer mMatrixGenerateTimer{};
  Timer mUploadToVBOTimer{};
  Timer mUploadToUBOTimer{};
  Timer mUIGenerateTimer{};
  Timer mUIDrawTimer{};

  Camera mCamera{};

  /* Mouse values. */
  bool mMouseLock = false;
  int mMouseXPos = 0;
  int mMouseYPos = 0;

  // Difference between current & previous draw() calls
  double mLastTickTime = 0.0;
};