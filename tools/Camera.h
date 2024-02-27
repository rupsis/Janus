#pragma once

#include "OGLRenderData.h"
#include <glm/glm.hpp>

class Camera {
 public:
  glm::mat4 getViewMatrix(OGLRenderData &renderData);

 private:
  //  Camera position in virtual world
  glm::vec3 mWorldPos = glm::vec3(0.5f, 0.25f, 1.0f);
  //   direction in which camera is facing
  glm::vec3 mViewDirection = glm::vec3(0.0f, 0.0f, 0.0f);
  //   Up facing vector (This is a Y up applications)
  glm::vec3 mWorldUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
};