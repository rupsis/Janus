#include <glm/gtc/type_ptr.hpp>

#include "SharedStorageBuffer.h"

void SharedStorageBuffer::init(size_t bufferSize) {
  mBufferSize = bufferSize;

  glGenBuffers(1, &mSharedStorageBuffer);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSharedStorageBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, mBufferSize, NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SharedStorageBuffer::uploadSsboData(std::vector<glm::mat4> bufferData, int bindingPoint) {
  if (bufferData.size() == 0) {
    // No data to upload
    return;
  }

  size_t bufferSize = bufferData.size() * sizeof(glm::mat4);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSharedStorageBuffer);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, bufferData.data());

  glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPoint, mSharedStorageBuffer, 0, bufferSize);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SharedStorageBuffer::cleanup() {
  glDeleteBuffers(1, &mSharedStorageBuffer);
}