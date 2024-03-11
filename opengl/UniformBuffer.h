#pragma once
#include <glm/glm.hpp>
#include <vector>

#include <glad/glad.h>

/* A Uniform Buffer is a read only buffer on the GPU
 * that shader _cannot_ write to. It's read only.
 */
class UniformBuffer {
 public:
  void init(size_t bufferSize);
  void uploadUboData(std::vector<glm::mat4> bufferData, int bindingPoint);
  void cleanup();

 private:
  size_t mBufferSize;
  GLuint mUboBuffer = 0;
};
