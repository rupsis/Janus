
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
  calculateBoneLengths();
  mFABRIKNodePositions.resize(mNodes.size());
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

void IKSolver::calculateBoneLengths() {
  mBoneLengths.resize(mNodes.size() - 1);

  for (int i = 0; i < mNodes.size() - 1; ++i) {
    std::shared_ptr<GltfNode> startNode = mNodes.at(i);
    std::shared_ptr<GltfNode> endNode = mNodes.at(i + 1);
    glm::vec3 startNodePos = startNode->getGlobalPosition();
    glm::vec3 endNodePos = endNode->getGlobalPosition();
    mBoneLengths.at(i) = glm::length(endNodePos - startNodePos);
    Logger::log(2, "%s: bone %i has length %f\n", __FUNCTION__, i, mBoneLengths.at(i));
  }
}

void IKSolver::solveFABRIKForward(glm::vec3 target) {
  mFABRIKNodePositions.at(0) = target;
  for (size_t i = 1; i < mFABRIKNodePositions.size(); ++i) {
    // calculate normalized direction
    glm::vec3 boneDirection = glm::normalize(mFABRIKNodePositions.at(i) -
                                             mFABRIKNodePositions.at(i - 1));

    // calculate offset of direction, and move bone to target.
    glm::vec3 offset = boneDirection * mBoneLengths.at(i - 1);
    mFABRIKNodePositions.at(i) = mFABRIKNodePositions.at(i - 1) + offset;
  }
}

void IKSolver::solveFABRIKBackward(glm::vec3 base) {
  mFABRIKNodePositions.at(mFABRIKNodePositions.size() - 1) = base;
  // work up the IK chain from the base
  for (int i = mFABRIKNodePositions.size() - 2; i >= 0; --i) {
    glm::vec3 boneDirection = glm::normalize(mFABRIKNodePositions.at(i) -
                                             mFABRIKNodePositions.at(i + 1));
    glm::vec3 offset = boneDirection * mBoneLengths.at(i);
    mFABRIKNodePositions.at(i) = mFABRIKNodePositions.at(i + 1) + offset;
  }
}

/* we need to ROTATE the bones, starting with the root node */
void IKSolver::adjustFABRIKNodes() {
  for (size_t i = mFABRIKNodePositions.size() - 1; i > 0; --i) {
    std::shared_ptr<GltfNode> node = mNodes.at(i);
    std::shared_ptr<GltfNode> nextNode = mNodes.at(i - 1);

    /* get the global position and rotation of the original nodes */
    glm::vec3 position = node->getGlobalPosition();
    glm::quat rotation = node->getGlobalRotation();

    /* calculate the vector of the original node direction */
    glm::vec3 nextPosition = nextNode->getGlobalPosition();
    glm::vec3 toNext = glm::normalize(nextPosition - position);

    /* calculate the vector of the changed node direction */
    glm::vec3 toDesired = glm::normalize(mFABRIKNodePositions.at(i - 1) -
                                         mFABRIKNodePositions.at(i));

    /* calculate the angle we have to rotate the node about */
    glm::quat nodeRotation = glm::rotation(toNext, toDesired);

    /* calculate the required local rotation from the world rotation */
    glm::quat localRotation = rotation * nodeRotation * glm::conjugate(rotation);

    /* rotate the node around the old plus the new rotation */
    glm::quat currentRotation = node->getLocalRotation();
    node->blendRotation(currentRotation * localRotation, 1.0f);

    /* update the node matrices, current node to effector
       to reflect the local changes down the chain */
    node->updateNodeAndChildMatrices();
  }
}

bool IKSolver::solveFABRIK(glm::vec3 target) {
  if (!mNodes.size()) {
    return false;
  }

  for (size_t i = 0; i < mNodes.size(); ++i) {
    std::shared_ptr<GltfNode> node = mNodes.at(i);
    mFABRIKNodePositions.at(i) = node->getGlobalPosition();
  }

  glm::vec3 base = getIkChainRootNode()->getGlobalPosition();

  for (unsigned int i = 0; i < mIterations; ++i) {
    glm::vec3 effector = mFABRIKNodePositions.at(0);
    if (glm::length(target - effector) < mThreshold) {
      adjustFABRIKNodes();
      return true;
    }

    solveFABRIKForward(target);
    solveFABRIKBackward(base);
  }

  adjustFABRIKNodes();

  glm::vec3 effector = mNodes.at(0)->getGlobalPosition();
  if (glm::length(target - effector) < mThreshold) {
    return true;
  }
  return false;
}