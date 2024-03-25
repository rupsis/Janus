#pragma once
#include <glm/glm.hpp>
#include <vector>

#include <glad/glad.h>

/* Shader Storage Buffer Objects (SSBOs) are a mix between
 * a uniform buffer and a texture. SSBO's can be much larger,
 * are writable, and can store arrays of arbitrary length.
 */
class ShaderStorageBuffer {
 public:
  void init(size_t bufferSize);
  void uploadSsboData(std::vector<glm::mat4> bufferData, int bindingPoint);
  void uploadSsboData(std::vector<glm::mat2x4> bufferData, int bindingPoint);
  void cleanup();

 private:
  size_t mBufferSize;
  GLuint mShaderStorageBuffer = 0;
};