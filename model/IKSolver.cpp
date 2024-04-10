
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "IKSolver.h"
#include "Logger.h"

IKSolver::IKSolver() : IKSolver(10) {}

IKSolver::IKSolver(unsigned int iterations) : mIterations(iterations) {}

void IKSolver::setNumIterations(unsigned int iterations) {
  mIterations = iterations;
}

void IKSolver::setNodes(std::vector<std::shared_ptr<GltfNode>> nodes) {
  mNodes = nodes;
  for (const auto &node : mNodes) {
    if (node) {
      Logger::log(
          2, "%s: added node %s to IK solver\n", __FUNCTION__, node->getNodeName().c_str());
    }
  }
}

std::shared_ptr<GltfNode> IKSolver::getIkChainRootNode() {
  return mNodes.at(mNodes.size() - 1);
}

bool IKSolver::solveCCD(const glm::vec3 target) {
  if (!mNodes.size()) {
    return false;
  }

  for (unsigned int i = 0; i < mIterations; ++i) {
    glm::vec3 effector = mNodes.at(0)->getGlobalPosition();
    if (glm::length(target - effector) <= mThreshold) {
      return true;
    }
    // Step over the effector for the forward solving
    for (size_t j = 1; j < mNodes.size(); j++) {
      std::shared_ptr<GltfNode> node = mNodes.at(j);
      if (!node) {
        continue;
      }
      // Get Nodes position and Rotation
      glm::vec3 position = node->getGlobalPosition();
      glm::quat rotation = node->getGlobalRotation();

      // calculate the direction from the node position to the effector
      glm::vec3 toEffector = glm::normalize(effector - position);
      // calculate the direction from the node position to the target
      glm::vec3 toTarget = glm::normalize(target - position);

      // calculate the global rotation that is required.
      glm::quat effectorToTarget = glm::rotation(toEffector, toTarget);

      // calculate the local rotation of the node.
      glm::quat localRotation = rotation * effectorToTarget * glm::conjugate(rotation);

      glm::quat currentRotation = node->getLocalRotation();
      node->blendRotation(currentRotation * localRotation, 1.0f);

      node->updateNodeAndChildMatrices();

      effector = mNodes.at(0)->getGlobalPosition();
      if (glm::length(target - effector) <= mThreshold) {
        return true;
      }
    }
  }
  return false;
}