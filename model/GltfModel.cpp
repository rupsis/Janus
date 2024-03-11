#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "GltfModel.h"
#include "Logger.h"

void GltfModel::createVertexBuffers() {
  // Model assumes only 1 mesh.
  const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);
  mVertexVBO.resize(primitives.attributes.size());
  mAttribAccessors.resize(primitives.attributes.size());

  for (const auto &attrib : primitives.attributes) {
    const std::string attribType = attrib.first;
    const int accessorNum = attrib.second;

    const tinygltf::Accessor &accessor = mModel->accessors.at(accessorNum);
    const tinygltf::BufferView &bufferView = mModel->bufferViews[accessor.bufferView];
    const tinygltf::Buffer &buffer = mModel->buffers[bufferView.buffer];

    // skip over attributes not containing Position, normals, or texture coordinates.
    if ((attribType.compare("POSITION") != 0) && (attribType.compare("NORMAL") != 0) &&
        (attribType.compare("TEXCOORD_0") != 0))
    {
      continue;
    }

    Logger::log(
        1, "%s: data for %s uses accessor %i\n", __FUNCTION__, attribType.c_str(), accessorNum);

    if (attribType.compare("POSITION") == 0) {
      int numPositionEntries = accessor.count;
      mAlteredPositions.resize(numPositionEntries);
      Logger::log(1, "%s: loaded %i vertices from glTF file\n", __FUNCTION__, numPositionEntries);
    }

    mAttribAccessors.at(attributes.at(attribType)) = accessorNum;

    int dataSize = 1;
    switch (accessor.type) {
      case TINYGLTF_TYPE_SCALAR:
        dataSize = 1;
        break;
      case TINYGLTF_TYPE_VEC2:
        dataSize = 2;
        break;
      case TINYGLTF_TYPE_VEC3:
        dataSize = 3;
        break;
      case TINYGLTF_TYPE_VEC4:
        dataSize = 4;
        break;
      default:
        Logger::log(
            1, "%s error: accessor %i uses data size %i\n", __FUNCTION__, accessorNum, dataSize);
        break;
    }

    GLuint dataType = GL_FLOAT;
    switch (accessor.componentType) {
      case TINYGLTF_COMPONENT_TYPE_FLOAT:
        dataType = GL_FLOAT;
        break;
      default:
        Logger::log(1,
                    "%s error: accessor %i uses unknown data type %i\n",
                    __FUNCTION__,
                    accessorNum,
                    dataType);
        break;
    }

    glGenBuffers(1, &mVertexVBO[attributes[attribType]]);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[attributes[attribType]]);
    glVertexAttribPointer(attributes[attribType], dataSize, dataType, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(attributes[attribType]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void GltfModel::createIndexBuffer() {
  glGenBuffers(1, &mIndexVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
  /* Note, we need to keep the buffer bound durring the
   * vertex array object creation, else it will crash on draw()
   */
}

void GltfModel::uploadVertexBuffers() {
  /*
   * accessor 0 = buffer with vertex position
   * accessor 1 = normal data
   * accessor 2 = texture coordinates
   */
  for (int i = 0; i < 3; ++i) {
    const tinygltf::Accessor &accessor = mModel->accessors.at(i);
    const tinygltf::BufferView &bufferView = mModel->bufferViews[accessor.bufferView];
    const tinygltf::Buffer &buffer = mModel->buffers[bufferView.buffer];

    glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[i]);
    glBufferData(GL_ARRAY_BUFFER,
                 bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void GltfModel::uploadIndexBuffer() {
  const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);
  const tinygltf::Accessor &indexAccessor = mModel->accessors.at(primitives.indices);
  const tinygltf::BufferView &indexBufferView = mModel->bufferViews[indexAccessor.bufferView];
  const tinygltf::Buffer &indexBuffer = mModel->buffers[indexBufferView.buffer];

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               indexBufferView.byteLength,
               &indexBuffer.data.at(0) + indexBufferView.byteOffset,
               GL_STATIC_DRAW);
}

void GltfModel::applyCPUVertexSkinning() {
  const tinygltf::Accessor &accessor = mModel->accessors.at(mAttribAccessors.at(0));
  const tinygltf::BufferView &bufferView = mModel->bufferViews.at(accessor.bufferView);
  const tinygltf::Buffer &buffer = mModel->buffers.at(bufferView.buffer);

  std::memcpy(
      mAlteredPositions.data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);

  for (int i = 0; i < mJointVec.size(); ++i) {
    glm::ivec4 jointIndex = glm::make_vec4(mJointVec.at(i));
    glm::vec4 weightIndex = glm::make_vec4(mWeightVec.at(i));
    glm::mat4 skinMat = weightIndex.x * mJointMatrices.at(jointIndex.x) +
                        weightIndex.y * mJointMatrices.at(jointIndex.y) +
                        weightIndex.z * mJointMatrices.at(jointIndex.z) +
                        weightIndex.w * mJointMatrices.at(jointIndex.w);
    mAlteredPositions.at(i) = skinMat * glm::vec4(mAlteredPositions.at(i), 1.0f);
  }
  glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO.at(0));
  glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength, mAlteredPositions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool GltfModel::loadModel(OGLRenderData &renderData,
                          std::string modelFilename,
                          std::string textureFilename) {
  if (!mTex.loadTexture(textureFilename, false)) {
    Logger::log(1, "%s: texture loading failed\n", __FUNCTION__);
    return false;
  }

  mModel = std::make_shared<tinygltf::Model>();

  tinygltf::TinyGLTF gltfLoader;
  std::string loaderErrors;
  std::string loaderWarnings;
  bool result = false;

  result = gltfLoader.LoadASCIIFromFile(
      mModel.get(), &loaderErrors, &loaderWarnings, modelFilename);

  if (!loaderWarnings.empty()) {
    Logger::log(
        1, "%s: warnings while loading glTF model: \n%s\n", __FUNCTION__, loaderWarnings.c_str());
  }

  if (!loaderErrors.empty()) {
    Logger::log(
        1, "%s: errors while loading glTF model: \n%s\n", __FUNCTION__, loaderErrors.c_str());
  }

  if (!result) {
    Logger::log(1, "%s error: could not load file '%s'\n", __FUNCTION__, modelFilename.c_str());
    return false;
  }

  // Once model is loaded, create vertex buffer & index buffer.
  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);

  /* extract position, normal, texture coords, and indices */
  createVertexBuffers();
  createIndexBuffer();

  glBindVertexArray(0);

  /* extract joints, weights, and invers bind matrices*/
  getJointData();
  getWeightData();
  getInvBindMatrices();

  /* build node tree from model.*/
  int nodeCount = mModel->nodes.size();
  int rootNode = mModel->scenes.at(0).nodes.at(0);

  Logger::log(1, "%s: model has %i nodes, root nodes is %i", __FUNCTION__, nodeCount, rootNode);

  mRootNode = GltfNode::createRoot(rootNode);
  getNodeData(mRootNode, glm::mat4(1.0f));
  getNodes(mRootNode);

  /* get Skeleton data */
  mSkeletonMesh = std::make_shared<OGLMesh>();

  mRootNode->printTree();

  renderData.rdTriangleCount = getTriangleCount();
  return true;
}

/* Getters. */
int GltfModel::getTriangleCount() {
  const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);
  const tinygltf::Accessor &indexAccessor = mModel->accessors.at(primitives.indices);
  return indexAccessor.count;
}

void GltfModel::getJointData() {
  std::string jointsAccessorAttrib = "JOINTS_0";
  int jointsAccessor = mModel->meshes.at(0).primitives.at(0).attributes.at(jointsAccessorAttrib);
  Logger::log(1,
              "%s: using accessor %i to get %s\n",
              __FUNCTION__,
              jointsAccessor,
              jointsAccessorAttrib.c_str());

  const tinygltf::Accessor &accessor = mModel->accessors.at(jointsAccessor);
  const tinygltf::BufferView &bufferView = mModel->bufferViews.at(accessor.bufferView);
  const tinygltf::Buffer &buffer = mModel->buffers.at(bufferView.buffer);

  int jointVecSize = accessor.count;
  Logger::log(1, "%s: %i short vec4 in JOINTS_0\n", __FUNCTION__, jointVecSize);
  mJointVec.resize(jointVecSize);

  std::memcpy(mJointVec.data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);

  mNodeToJoint.resize(mModel->nodes.size());

  const tinygltf::Skin &skin = mModel->skins.at(0);
  for (int i = 0; i < skin.joints.size(); ++i) {
    int destinationNode = skin.joints.at(i);
    mNodeToJoint.at(destinationNode) = i;
    Logger::log(2, "%s: joint %i affects node %i\n", __FUNCTION__, i, destinationNode);
  }
}

void GltfModel::getWeightData() {
  std::string weightsAccessorAttrib = "WEIGHTS_0";
  int weightAccessor = mModel->meshes.at(0).primitives.at(0).attributes.at(weightsAccessorAttrib);
  Logger::log(1,
              "%s: using accessor %i to get %s\n",
              __FUNCTION__,
              weightAccessor,
              weightsAccessorAttrib.c_str());

  const tinygltf::Accessor &accessor = mModel->accessors.at(weightAccessor);
  const tinygltf::BufferView &bufferView = mModel->bufferViews.at(accessor.bufferView);
  const tinygltf::Buffer &buffer = mModel->buffers.at(bufferView.buffer);

  int weightVecSize = accessor.count;
  Logger::log(1, "%s: %i vec4 in WEIGHTS_0\n", __FUNCTION__, weightVecSize);
  mWeightVec.resize(weightVecSize);

  std::memcpy(
      mWeightVec.data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
}

void GltfModel::getInvBindMatrices() {
  const tinygltf::Skin &skin = mModel->skins.at(0);
  int invBindMatAccessor = skin.inverseBindMatrices;

  const tinygltf::Accessor &accessor = mModel->accessors.at(invBindMatAccessor);
  const tinygltf::BufferView &bufferView = mModel->bufferViews.at(accessor.bufferView);
  const tinygltf::Buffer &buffer = mModel->buffers.at(bufferView.buffer);

  mInverseBindMatrices.resize(skin.joints.size());
  mJointMatrices.resize(skin.joints.size());
  mJointDualQuats.resize(skin.joints.size());

  std::memcpy(mInverseBindMatrices.data(),
              &buffer.data.at(0) + bufferView.byteOffset,
              bufferView.byteLength);
}

int GltfModel::getJointMatrixSize() {
  return mJointMatrices.size();
}

std::vector<glm::mat4> GltfModel::getJointMatrices() {
  return mJointMatrices;
}

void GltfModel::getNodes(std::shared_ptr<GltfNode> treeNode) {
  int nodeNum = treeNode->getNodeNum();
  std::vector<int> childNodes = mModel->nodes.at(nodeNum).children;

  /* Remove any children nodes with skin/mesh metadata.*/
  auto removed = std::remove_if(childNodes.begin(), childNodes.end(), [&](int num) {
    return mModel->nodes.at(num).skin != -1;
  });
  childNodes.erase(removed, childNodes.end());

  treeNode->addChilds(childNodes);
  glm::mat4 treeNodeMatrix = treeNode->getNodeMatrix();

  for (auto &childNode : treeNode->getChilds()) {
    getNodeData(childNode, treeNodeMatrix);
    getNodes(childNode);
  }
}

void GltfModel::getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix) {

  int nodeNum = treeNode->getNodeNum();
  const tinygltf::Node &node = mModel->nodes.at(nodeNum);
  treeNode->setNodeName(node.name);

  if (node.translation.size()) {
    treeNode->setTranslation(glm::make_vec3(node.translation.data()));
  }
  if (node.rotation.size()) {
    treeNode->setRotation(glm::make_quat(node.rotation.data()));
  }
  if (node.scale.size()) {
    treeNode->setScale(glm::make_vec3(node.scale.data()));
  }

  treeNode->calculateLocalTRSMatrix();
  treeNode->calculateNodeMatrix(parentNodeMatrix);

  mJointMatrices.at(mNodeToJoint.at(nodeNum)) = treeNode->getNodeMatrix() *
                                                mInverseBindMatrices.at(mNodeToJoint.at(nodeNum));

  /* Need to decompose join matrix into components to convert to quaternions. */
  glm::quat orientation;
  glm::vec3 scale;
  glm::vec3 translation;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::dualquat dq;

  if (glm::decompose(mJointMatrices.at(mNodeToJoint.at(nodeNum)),
                     scale,
                     orientation,
                     translation,
                     skew,
                     perspective))
  {
    dq[0] = orientation;
    dq[1] = glm::quat(0.0, translation.x, translation.y, translation.z) * orientation * 0.5f;
    mJointDualQuats.at(mNodeToJoint.at(nodeNum)) = glm::mat2x4_cast(dq);
  }
}

std::shared_ptr<OGLMesh> GltfModel::getSkeleton(bool enableSkinning) {
  mSkeletonMesh->vertices.resize(mModel->nodes.size() * 2);
  mSkeletonMesh->vertices.clear();

  /* start from Armature child */
  getSkeletonPerNode(mRootNode->getChilds().at(0), enableSkinning);
  return mSkeletonMesh;
}

void GltfModel::getSkeletonPerNode(std::shared_ptr<GltfNode> treeNode, bool enableSkinning) {
  glm::vec3 parentPos = glm::vec3(0.0f);
  if (enableSkinning) {
    parentPos = glm::vec3(treeNode->getNodeMatrix() * glm::vec4(1.0f));
  }
  else {
    glm::mat4 bindMatrix = glm::inverse(
        mInverseBindMatrices.at(mNodeToJoint.at(treeNode->getNodeNum())));
    parentPos = bindMatrix * treeNode->getNodeMatrix() * glm::vec4(1.0f);
  }
  OGLVertex parentVertex;
  parentVertex.position = parentPos;
  parentVertex.color = glm::vec3(0.0f, 1.0f, 1.0f);

  for (const auto &childNode : treeNode->getChilds()) {
    glm::vec3 childPos = glm::vec3(0.0f);
    if (enableSkinning) {
      childPos = glm::vec3(childNode->getNodeMatrix() * glm::vec4(1.0f));
    }
    else {
      glm::mat4 bindMatrix = glm::inverse(
          mInverseBindMatrices.at(mNodeToJoint.at(childNode->getNodeNum())));
      childPos = bindMatrix * childNode->getNodeMatrix() * glm::vec4(1.0f);
    }
    OGLVertex childVertex;
    childVertex.position = childPos;
    childVertex.color = glm::vec3(0.0f, 0.0f, 1.0f);
    mSkeletonMesh->vertices.emplace_back(parentVertex);
    mSkeletonMesh->vertices.emplace_back(childVertex);

    getSkeletonPerNode(childNode, enableSkinning);
  }
}

/* ------ */

void GltfModel::draw() {
  const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);
  const tinygltf::Accessor &indexAccessor = mModel->accessors.at(primitives.indices);

  GLuint drawMode = GL_TRIANGLES;
  switch (primitives.mode) {
    case TINYGLTF_MODE_TRIANGLES:
      drawMode = GL_TRIANGLES;
      break;
    default:
      Logger::log(1, "%s error: unknown draw mode %i\n", __FUNCTION__, drawMode);
      break;
  }

  mTex.bind();
  glBindVertexArray(mVAO);
  // We have indexed geometry, instead of array data
  glDrawElements(drawMode, indexAccessor.count, indexAccessor.componentType, nullptr);

  glBindVertexArray(0);
  mTex.unbind();
}

void GltfModel::cleanup() {
  glDeleteBuffers(mVertexVBO.size(), mVertexVBO.data());
  glDeleteBuffers(1, &mVAO);
  glDeleteBuffers(1, &mIndexVBO);
  mTex.cleanup();
  mModel.reset();
}
