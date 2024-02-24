#include <glm/gtc/type_ptr.hpp>

#include "UniformBuffer.h"

void UniformBuffer::init() {
  // Create new OpenGL buffer
  glGenBuffers(1, &mUboBuffer);
  // Bind buffer to local for modification
  glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);
  // allocate memory for 2 4x4 matrices
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  // Unbind buffer after modifications
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::uploadUboData(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
  glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);

  // (Buffer type, size offset, size of data, pointer to data)
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));

  glBufferSubData(
      GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, mUboBuffer, 0, 2 * sizeof(glm::mat4));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::cleanup() {
  glDeleteBuffers(1, &mUboBuffer);
}