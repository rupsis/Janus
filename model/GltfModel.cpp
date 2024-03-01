#include "GltfModel.h"
#include "Logger.h"

void GltfModel::createVertexBuffers() {
  // Model assumes only 1 mesh.
  const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);

  mVertexVBO.resize(primitives.attributes.size());
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

int GltfModel::getTriangleCount() {
  const tinygltf::Primitive &primitives = mModel->meshes.at(0).primitives.at(0);
  const tinygltf::Accessor &indexAccessor = mModel->accessors.at(primitives.indices);
  return indexAccessor.count;
}

bool GltfModel::loadModel(OGLRenderData &renderData,
                          std::string modelFilename,
                          std::string textureFilename) {
  if (!mTex.loadTexture(textureFilename, false)) {
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
  createVertexBuffers();
  createIndexBuffer();
  glBindVertexArray(0);

  renderData.rdTriangleCount = getTriangleCount();
  return true;
}

void GltfModel::cleanup() {
  glDeleteBuffers(mVertexVBO.size(), mVertexVBO.data());
  glDeleteBuffers(1, &mVAO);
  glDeleteBuffers(1, &mIndexVBO);
  mTex.cleanup();
  mModel.reset();
}

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