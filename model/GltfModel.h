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
  void uploadIndexBuffer();

 private:
  void createVertexBuffers();
  void createIndexBuffer();
  int getTriangleCount();

  /* Armature. */
  void getNodes(std::shared_ptr<GltfNode> treeNode);
  void getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);

  std::vector<glm::tvec4<uint16_t>> mJointVec{};
  std::vector<glm::vec4> mWeightVec{};
  std::vector<glm::mat4> mInverseBindMatrices{};
  std::vector<glm::mat4> mJointMatrices{};

  std::vector<int> mAttribAccessors{};
  std::vector<int> mNodeToJoint{};

  std::vector<glm::vec3> mAlteredPositions{};

  std::shared_ptr<GltfNode> mRootNode = nullptr;
  std::shared_ptr<tinygltf::Model> mModel = nullptr;

  GLuint mVAO = 0;
  std::vector<GLuint> mVertexVBO{};
  GLuint mIndexVBO = 0;

  std::map<std::string, GLint> attributes = {{"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}};
  Texture mTex{};
};