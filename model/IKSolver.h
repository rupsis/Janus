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

  bool solveCCD(glm::vec3 target);

 private:
  std::vector<std::shared_ptr<GltfNode>> mNodes{};
  unsigned int mIterations = 0;
  float mThreshold = 0.00001f;
};
