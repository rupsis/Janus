#include "UniformBuffer.h"
#include "Logger.h"

void UniformBuffer::init(size_t bufferSize) {
  mBufferSize = bufferSize;
  // Create new OpenGL buffer
  glGenBuffers(1, &mUboBuffer);
  // Bind buffer to local for modification
  glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);
  // allocate memory for 4x4 matrices
  glBufferData(GL_UNIFORM_BUFFER, mBufferSize, NULL, GL_STATIC_DRAW);
  // Unbind buffer after modifications
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::uploadUboData(std::vector<glm::mat4> bufferData, int bindingPoint) {
  if (bufferData.size() == 0) {
    return;
  }
  size_t bufferSize = bufferData.size() * sizeof(glm::mat4);
  glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);

  // (Buffer type, size offset, size of data, pointer to data)
  glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, bufferData.data());
  glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, mUboBuffer, 0, bufferSize);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::cleanup() {
  glDeleteBuffers(1, &mUboBuffer);
}
