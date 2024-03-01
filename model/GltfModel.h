#pragma once

#include <glad/glad.h>
#include <memory>
#include <string>
#include <tiny_gltf.h>
#include <vector>

#include "Texture.h"

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

  std::shared_ptr<tinygltf::Model> mModel = nullptr;
  GLuint mVAO = 0;
  std::vector<GLuint> mVertexVBO{};
  GLuint mIndexVBO = 0;

  std::map<std::string, GLint> attributes = {{"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}};
  Texture mTex{};
};