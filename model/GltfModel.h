#pragma once

#include <glad/glad.h>
#include <memory>
#include <string>
#include <tiny_gltf.h>
#include <vector>

#include "Texture.h"

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
  void applyCPUVertexSkinning();
  void uploadIndexBuffer();
  std::shared_ptr<OGLMesh> getSkeleton(bool enableSkinning);
  int getJointMatrixSize();
  std::vector<glm::mat4> getJointMatrices();

 private:
  void createVertexBuffers();
  void createIndexBuffer();
  int getTriangleCount();

  /* Armature. */
  void getJointData();
  void getWeightData();
  void getInvBindMatrices();
  void getNodes(std::shared_ptr<GltfNode> treeNode);
  void getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);
  void getSkeletonPerNode(std::shared_ptr<GltfNode> treeNode, bool enableSkinning);

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

  GLuint mVAO = 0;
  std::vector<GLuint> mVertexVBO{};
  GLuint mIndexVBO = 0;

  std::map<std::string, GLint> attributes = {
      {"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}, {"JOINTS_0", 3}, {"WEIGHTS_0", 4}};
  Texture mTex{};
};