#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

/* Due to the usage of GLM, the data is organized in system memory
 * the same way it would be in GPU memory, allowing a simple copy to transfer vertex data.
 */
struct OGLVertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 uv;
};

struct OGLMesh {
  std::vector<OGLVertex> vertices;
};

struct OGLRenderData {
  GLFWwindow *rdWindow = nullptr;

  unsigned int rdHeight = 0;
  unsigned int rdWidth = 0;

  unsigned int rdTriangleCount = 0;
  float rdFrameTime = 0.0f;
  float rdUIGenerateTime = 0.0f;

  bool rdUseChangedShader = false;
  bool rdGPUVertexSkinning = false;
  int rdFieldOfView = 90;

  // Camera info
  float rdViewAzimuth = 320.0f;
  float rdViewElevation = -15.0f;

  glm::vec3 rdCameraWorldPosition = glm::vec3(0.5f, 0.25f, 1.0f);

  int rdMoveForward = 0;
  int rdMoveRight = 0;
  int rdMoveUp = 0;

  float rdTickDiff = 0.0f;

  unsigned int rdGltfTriangleCount = 0;
};