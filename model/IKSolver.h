#pragma once
#include "GltfNode.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class IKSolver {
 public:
  IKSolver();
  IKSolver(unsigned int iterations);

  void setNodes(std::vector<std::shared_ptr<GltfNode>> nodes);
  std::shared_ptr<GltfNode> getIkChainRootNode();

  void setNumIterations(unsigned int iterations);

  // CCD - Cycle Coordinate Descent
  bool solveCCD(glm::vec3 target);

  // FABRIK Forward And Backward Reaching Inverse Kinematics
  bool solveFABRIK(glm::vec3 target);

 private:
  void solveFABRIKForward(glm::vec3 target);
  void solveFABRIKBackward(glm::vec3 base);

  void calculateBoneLengths();

  void adjustFABRIKNodes();

  std::vector<std::shared_ptr<GltfNode>> mNodes{};
  std::vector<float> mBoneLengths{};
  std::vector<glm::vec3> mFABRIKNodePositions{};
  unsigned int mIterations = 0;
  float mThreshold = 0.00001f;
};
