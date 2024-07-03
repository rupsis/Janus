#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class GltfNode : public std::enable_shared_from_this<GltfNode> {

 public:
  static std::shared_ptr<GltfNode> createRoot(int rootNodeNum);
  void addChilds(std::vector<int> childNodes);

  std::vector<std::shared_ptr<GltfNode>> getChilds();
  int getNodeNum();
  std::string getNodeName();
  std::shared_ptr<GltfNode> getParentNode();
  glm::quat getLocalRotation();
  glm::quat getGlobalRotation();
  glm::vec3 getGlobalPosition();

  void updateNodeAndChildMatrices();

  void setNodeName(std::string name);
  void setScale(glm::vec3 scale);
  void setTranslation(glm::vec3 translation);
  void setRotation(glm::quat rotation);

  void blendScale(glm::vec3 scale, float blendFactor);
  void blendTranslation(glm::vec3 translation, float blendFactor);
  void blendRotation(glm::quat rotation, float blendFactor);

  void calculateLocalTRSMatrix();
  void calculateNodeMatrix();
  glm::mat4 getNodeMatrix();

  void printTree();

 private:
  void printNodes(std::shared_ptr<GltfNode> startNode, int indent);

  int mNodeNum = 0;
  std::string mNodeName;

  // Use weak pointer here to avoid circular dependency.
  // weak pointer doesn't count toward reference counter.
  std::weak_ptr<GltfNode> mParentNode;

  std::vector<std::shared_ptr<GltfNode>> mChildNodes{};

  glm::vec3 mScale = glm::vec3(1.0f);
  glm::vec3 mTranslation = glm::vec3(0.0f);
  glm::quat mRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  /* Animation Blend Values */
  glm::vec3 mBlendScale = glm::vec3(1.0);
  glm::vec3 mBlendTranslation = glm::vec3(0.0f);
  glm::quat mBlendRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  glm::mat4 mLocalTRSMatrix = glm::mat4(1.0f);
  glm::mat4 mNodeMatrix = glm::mat4(1.0f);
  glm::mat4 mInverseBindMatrix = glm::mat4(1.0f);
};