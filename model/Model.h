#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "OGLRenderData.h"

class Model {
 public:
  void init();

  OGLMesh getVertexData();

 private:
  OGLMesh mVertexData;
};