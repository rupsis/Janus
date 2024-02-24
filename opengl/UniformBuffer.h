#pragma once
#include <glm/glm.hpp>

#include <glad/glad.h>

/* A Uniform Buffer is a read only buffer on the GPU
 * that shader _cannot_ write to. It's read only.
 */
class UniformBuffer {
 public:
  void init();
  void uploadUboData(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
  void cleanup();

 private:
  GLuint mUboBuffer = 0;
};