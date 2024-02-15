#include "VertexBuffer.h"

void VertexBuffer::init() {
  // New vertex array object. This contains the vertex buffer object.
  glGenVertexArrays(1, &mVAO);
  // New vertex buffer object. This contains the vertex and texture data.
  glGenBuffers(1, &mVertexVBO);

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);

  // We need to cast the offset values to void * to match the signature of the call.
  // offsetof macro is used to get the offsets of the position
  // and the texture coordinates inside the struct.
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void *)offsetof(OGLVertex, position));
  glVertexAttribPointer(
      1, 2, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void *)offsetof(OGLVertex, uv));

  // Enable the 2 buffers we just configured.
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void VertexBuffer::cleanup() {
  glDeleteBuffers(1, &mVertexVBO);
  glDeleteVertexArrays(1, &mVAO);
}

void VertexBuffer::uploadData(OGLMesh vertexData) {
  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);
  glBufferData(GL_ARRAY_BUFFER,
               vertexData.vertices.size() * sizeof(OGLVertex),
               &vertexData.vertices.at(0),
               GL_DYNAMIC_DRAW);

  glBindVertexArray(0);
}

void VertexBuffer::bind() {
  glBindVertexArray(mVAO);
}

void VertexBuffer::unbind() {
  glBindVertexArray(0);
}

/*
 * Call OpenGL draw on the vertex array from the currently bound vertex array object,
 * starting at index 'start' index and rendering 'num' elements.
 */
void VertexBuffer::draw(GLuint mode, unsigned int start, unsigned int num) {
  glDrawArrays(mode, start, num);
}