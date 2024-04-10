#pragma once

#include <glad/glad.h>
#include <memory>
#include <string>
#include <tiny_gltf.h>
#include <vector>

#include "Texture.h"

#include "IKSolver.h"

#include "GltfAnimationClip.h"
#include "GltfNode.h"

#include "OGLRenderData.h"

class GltfModel {
 public:
  bool loadModel(OGLRenderData &renderData,
                 std::string modelFilename,
                 std::string textureFilename);
  void draw();
  void cleanup();
  void uploadVertexBuffers();
  void uploadIndexBuffer();

  std::shared_ptr<OGLMesh> getSkeleton();
  void setSkeletonSplitNode(int nodeNum);
  int getJointMatrixSize();

  std::vector<glm::mat4> getJointMatrices();
  int getJointDualQuatsSize();
  std::vector<glm::mat2x4> getJointDualQuats();
  std::string getNodeName(int nodeNum);

  /* Inverse Kinematics */
  void setInverseKinematicsNodes(int effectorNodeNum, int ikChainRootNodeNum);

  void setNumIKIterations(int iterations);

  // CCD Cyclic Coordinate Descent
  void solveIKByCCD(glm::vec3 target);

  /* Animations */
  void playAnimation(int animNum,
                     float speedDivider,
                     float blendFactor,
                     replayDirection direction);

  void playAnimation(int sourceAnimNum,
                     int destAnimNum,
                     float speedDivider,
                     float blendFactor,
                     replayDirection direction);

  void blendAnimationFrame(int animNumber, float time, float blendFactor);
  void crossBlendAnimationFrame(int sourceAnimNumber,
                                int destAnimNumber,
                                float time,
                                float blendFactor);
  float getAnimationEndTime(int animNum);
  std::string getClipName(int animNum);
  void getAnimations();

  void resetNodeData();

 private:
  void createVertexBuffers();
  void createIndexBuffer();
  int getTriangleCount();
  void getSkeletonPerNode(std::shared_ptr<GltfNode> treeNode);

  /* Armature. */
  void getJointData();
  void getWeightData();
  void getInvBindMatrices();
  void getNodes(std::shared_ptr<GltfNode> treeNode);
  void getNodeData(std::shared_ptr<GltfNode> treeNode);
  void resetNodeData(std::shared_ptr<GltfNode> treeNode);
  void updateNodeMatrices(std::shared_ptr<GltfNode> treeNode);
  void updateJointMatricesAndQuats(std::shared_ptr<GltfNode> treeNode);
  void updateAdditiveMask(std::shared_ptr<GltfNode> treeNode, int splitNodeNum);

  // Joint data is hardcoded to the test model.
  // Will need to check componentType field to convert
  // the joint data to the data type used in the shader during model creation.
  std::vector<glm::tvec4<uint16_t>> mJointVec{};
  std::vector<glm::vec4> mWeightVec{};
  std::vector<glm::mat4> mInverseBindMatrices{};
  std::vector<glm::mat4> mJointMatrices{};
  std::vector<glm::mat2x4> mJointDualQuats{};

  std::vector<int> mAttribAccessors{};
  std::vector<int> mNodeToJoint{};

  std::vector<glm::vec3> mAlteredPositions{};

  std::shared_ptr<GltfNode> mRootNode = nullptr;
  std::shared_ptr<tinygltf::Model> mModel = nullptr;

  std::shared_ptr<OGLMesh> mSkeletonMesh = nullptr;

  std::vector<std::shared_ptr<GltfNode>> mNodeList;

  std::vector<bool> mAdditiveAnimationMask{};
  std::vector<bool> mInvertedAdditiveAnimationMask{};

  // Animation
  std::vector<std::shared_ptr<GltfAnimationClip>> mAnimClips{};

  GLuint mVAO = 0;
  std::vector<GLuint> mVertexVBO{};
  GLuint mIndexVBO = 0;

  std::map<std::string, GLint> attributes = {
      {"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}, {"JOINTS_0", 3}, {"WEIGHTS_0", 4}};
  Texture mTex{};

  IKSolver mIKSolver{};
};