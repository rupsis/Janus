#include "OGLRenderer.h"
#include "Logger.h"

bool OGLRenderer::init(unsigned int width, unsigned int height) {
  Logger::log(1, "%s: OpenGL Render Inti \n", __FUNCTION__);
  // Initialize OpenGL via Glad.
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    return false;
  }

  if (!GLAD_GL_VERSION_4_6) {
    return false;
  }

  if (!mFramebuffer.init(width, height)) {
    return false;
  }

  if (!mTex.loadTexture("textures/crate.png")) {
    return false;
  }

  mVertexBuffer.init();

  if (!mShader.loadShaders("shader/basic.vert", "shader/basic.frag")) {
    return false;
  }
  return true;
}

void OGLRenderer::setSize(unsigned int width, unsigned int height) {
  mFramebuffer.resize(width, height);
  glViewport(0, 0, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData) {
  Logger::log(1, "%s: OpenGL Render uploadData \n", __FUNCTION__);
  mTriangleCount = vertexData.vertices.size();
  mVertexBuffer.uploadData(vertexData);
}

void OGLRenderer::draw() {
  Logger::log(1, "%s: OpenGL Render draw \n", __FUNCTION__);
  // Bind buffer to let us receive vertex data.
  mFramebuffer.bind();
  glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);

  // draw triangle from buffer
  mShader.use();
  mTex.bind();
  mVertexBuffer.bind();
  mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount);
  mVertexBuffer.unbind();
  mTex.unbind();
  mFramebuffer.unbind();
  mFramebuffer.drawToScreen();
}

void OGLRenderer::cleanup() {
  mShader.cleanup();
  mTex.cleanup();
  mVertexBuffer.cleanup();
  mFramebuffer.cleanup();
}