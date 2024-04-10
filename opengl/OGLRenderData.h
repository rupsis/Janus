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

/* UI Ratio button enums*/
enum class skinningMode { linear = 0, dualQuat };

enum class blendMode { fadeInOut = 0, crossFade, additive };

enum class replayDirection { forward = 0, backward };

/* Inverse Kinematics. */
enum class ikMode { off = 0, ccd };

struct OGLRenderData {
  GLFWwindow *rdWindow = nullptr;

  int rdHeight = 0;
  int rdWidth = 0;

  unsigned int rdTriangleCount = 0;
  unsigned int rdGltfTriangleCount = 0;

  float rdFrameTime = 0.0f;
  float rdMatrixGenerateTime = 0.0f;
  float rdIKTime = 0.0f;
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

  /* Animation */
  bool rdPlayAnimation = true;
  float rdAnimBlendFactor = 1.0f;
  std::vector<std::string> rdClipNames{};
  int rdAnimClip = 0;
  int rdAnimClipSize = 0;
  float rdAnimSpeed = 1.0f;
  float rdAnimTimePosition = 0.0f;
  float rdAnimEndTime = 0.0f;

  int rdCrossBlendDestAnimClip = 0;
  float rdAnimCrossBlendFactor = 0.0f;

  /* Inverse Kinematics.*/
  ikMode rdIkMode = ikMode::off;
  int rdIkIterations = 10;
  glm::vec3 rdIkTargetPos = glm::vec3(0.0f, 3.0f, 1.0f);
  int rdIkEffectorNode = 0;
  int rdIkRootNode = 0;

  int rdModelNodeCount = 0;
  int rdSkelSplitNode = 0;
  std::vector<std::string> rdSkelNodeNames{};

  skinningMode rdGPUDualQuatVertexSkinning = skinningMode::linear;
  blendMode rdBlendingMode = blendMode::fadeInOut;
  replayDirection rdAnimationPlayDirection = replayDirection::forward;
};