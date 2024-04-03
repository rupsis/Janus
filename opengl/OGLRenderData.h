#pragma once

#include <glm/glm.hpp>
#include <string>
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

  int rdHeight = 0;
  int rdWidth = 0;

  unsigned int rdTriangleCount = 0;
  unsigned int rdGltfTriangleCount = 0;

  float rdFrameTime = 0.0f;
  float rdMatrixGenerateTime = 0.0f;
  float rdUploadToVBOTime = 0.0f;
  float rdUploadToUBOTime = 0.0f;
  float rdUIGenerateTime = 0.0f;
  float rdUIDrawTime = 0.0f;

  // Camera info
  float rdViewAzimuth = 0.0f;
  float rdViewElevation = 0.0f;
  int rdFieldOfView = 60;

  glm::vec3 rdCameraWorldPosition = glm::vec3(-0.5f, 2.5f, 6.0f);

  int rdMoveForward = 0;
  int rdMoveRight = 0;
  int rdMoveUp = 0;

  float rdTickDiff = 0.0f;

  bool rdDrawGltfModel = true;
  bool rdDrawSkeleton = true;
  bool rdGPUDualQuatVertexSkinning = false;

  /* Animation */
  bool rdPlayAnimation = true;
  float rdAnimBlendFactor = 1.0f;
  std::string rdClipName = "None";
  int rdAnimClip = 0;
  int rdAnimClipSize = 0;
  float rdAnimSpeed = 1.0f;
  float rdAnimTimePosition = 0.0f;
  float rdAnimEndTime = 0.0f;

  bool rdCrossBlending = false;
  int rdCrossBlendDestAnimClip = 0;
  std::string rdCrossBlendDestClipName = "None";
  float rdAnimCrossBlendFactor = 0.0f;

  int rdModelNodeCount = 0;
  bool rdAdditiveBlending = false;
  int rdSkelSplitNode = 0;
  std::string rdSkelSplitNodeName = "None";
};