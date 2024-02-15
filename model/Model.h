#pragma once
#include "OGLRenderData.h"
#include <glm/glm.hpp>
#include <vector>

class Model {
 public:
  void init();
  OGLMesh getVertexData();

 private:
  OGLMesh mVertexData{};
};