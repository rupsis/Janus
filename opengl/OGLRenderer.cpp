#include <glm/gtc/matrix_transform.hpp>

#include "Logger.h"
#include "OGLRenderer.h"

OGLRenderer::OGLRenderer(GLFWwindow *window) {
  mWindow = window;
}

bool OGLRenderer::init(unsigned int width, unsigned int height) {
  /* required for perspective */
  mWidth = width;
  mHeight = height;

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

  if (!mBasicShader.loadShaders("shader/basic.vert", "shader/basic.frag")) {
    return false;
  }

  if (!mChangedShader.loadShaders("shader/changed.vert", "shader/changed.frag")) {
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

  glm::vec3 cameraPosition = glm::vec3(0.4f, 0.3f, 1.0f);
  glm::vec3 cameraLookAtPosition = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 cameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

  mProjectionMatrix = glm::perspective(
      glm::radians(90.0f), static_cast<float>(mWidth) / static_cast<float>(mHeight), 0.1f, 100.f);

  float t = glfwGetTime();

  glm::mat4 view = glm::mat4(1.0);

  // draw triangle from buffer
  if (mUseChangedShader) {
    glm::vec3 offset = glm::vec3(0.0, 0.0, -1.0);
    view = glm::rotate(glm::mat4(1.0f), -t, glm::vec3(0.0f, 0.0f, 1.0f) * offset);
    mChangedShader.use();
  }
  else {
    view = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0f, 0.0f, 1.0f));
    mBasicShader.use();
  }
  mViewMatrix = glm::lookAt(cameraPosition, cameraLookAtPosition, cameraUpVector) * view;
  mUniformBuffer.uploadUboData(mViewMatrix, mProjectionMatrix);

  mTex.bind();
  mVertexBuffer.bind();
  mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount);
  mVertexBuffer.unbind();
  mTex.unbind();
  mFramebuffer.unbind();
  mFramebuffer.drawToScreen();
}

void OGLRenderer::cleanup() {
  mBasicShader.cleanup();
  mChangedShader.cleanup();
  mTex.cleanup();
  mVertexBuffer.cleanup();
  mFramebuffer.cleanup();
}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods) {
  Logger::log(1, "%s: Render key handle event \n", __FUNCTION__);

  if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
    Logger::log(1, "%s: shader change key handle event \n", __FUNCTION__);
    mUseChangedShader = !mUseChangedShader;
  }
}