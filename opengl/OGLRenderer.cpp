#include <glm/gtc/matrix_transform.hpp>
#include <imgui_impl_glfw.h>

#include "Logger.h"
#include "OGLRenderer.h"

#include <iostream>

OGLRenderer::OGLRenderer(GLFWwindow *window) {
  mRenderData.rdWindow = window;
}

bool OGLRenderer::init(unsigned int width, unsigned int height) {
  /* required for perspective */
  mRenderData.rdWidth = width;
  mRenderData.rdHeight = height;

  /* initalize GLAD */
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Logger::log(1, "%s error: failed to initialize GLAD\n", __FUNCTION__);
    return false;
  }

  if (!GLAD_GL_VERSION_4_6) {
    Logger::log(1, "%s error: failed to get at least OpenGL 4.6\n", __FUNCTION__);
    return false;
  }

  GLint majorVersion, minorVersion;
  glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
  glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
  Logger::log(1, "%s: OpenGL %d.%d initializeed\n", __FUNCTION__, majorVersion, minorVersion);

  if (!mFramebuffer.init(width, height)) {
    Logger::log(1, "%s error: could not init Framebuffer\n", __FUNCTION__);
    return false;
  }
  Logger::log(1, "%s: framebuffer succesfully initialized\n", __FUNCTION__);

  mVertexBuffer.init();
  Logger::log(1, "%s: vertex buffer successfully created\n", __FUNCTION__);

  size_t uniformMatrixBufferSize = 2 * sizeof(glm::mat4);
  mUniformBuffer.init(uniformMatrixBufferSize);
  Logger::log(1, "%s: matrix uniform buffer (size %i bytes) successfully created\n", __FUNCTION__, uniformMatrixBufferSize);

  if (!mLineShader.loadShaders("shader/line.vert", "shader/line.frag")) {
    Logger::log(1, "%s: line shader loading failed\n", __FUNCTION__);
    return false;
  }
  if (!mGltfGPUShader.loadShaders("shader/gltf_gpu.vert", "shader/gltf_gpu.frag")) {
    Logger::log(1, "%s: gltTF GPU shader loading failed\n", __FUNCTION__);
    return false;
  }
  if (!mGltfGPUDualQuatShader.loadShaders("shader/gltf_gpu_dquat.vert",
      "shader/gltf_gpu_dquat.frag")) {
    Logger::log(1, "%s: glTF GPU dual quat shader loading failed\n", __FUNCTION__);
    return false;
  }
  Logger::log(1, "%s: shaders succesfully loaded\n", __FUNCTION__);

  mUserInterface.init(mRenderData);
  Logger::log(1, "%s: user interface initialized\n", __FUNCTION__);

  /* add backface culling and depth test already here */
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glLineWidth(3.0);

  mGltfModel = std::make_shared<GltfModel>();
  std::string modelFilename = "assets/Woman.gltf";
  std::string modelTexFilename = "textures/Woman.png";
  if (!mGltfModel->loadModel(mRenderData, modelFilename, modelTexFilename)) {
    Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
    return false;
  }
  mGltfModel->uploadIndexBuffer();
  Logger::log(1, "%s: glTF model '%s' succesfully loaded\n", __FUNCTION__, modelFilename.c_str());

  size_t modelJointMatrixBufferSize = mGltfModel->getJointMatrixSize() * sizeof(glm::mat4);
  mGltfShaderStorageBuffer.init(modelJointMatrixBufferSize);
  Logger::log(1, "%s: glTF joint matrix shader storage buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointMatrixBufferSize);

  size_t modelJointDualQuatBufferSize = mGltfModel->getJointDualQuatsSize() *
    sizeof(glm::mat2x4);
  mGltfDualQuatSSBuffer.init(modelJointDualQuatBufferSize);
  Logger::log(1, "%s: glTF joint dual quaternions shader storage buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointDualQuatBufferSize);

  /* valid, but emtpy */
  mSkeletonMesh = std::make_shared<OGLMesh>();
  Logger::log(1, "%s: skeleton mesh storage initialized\n", __FUNCTION__);

  mFrameTimer.start();

  return true;
}

void OGLRenderer::setSize(unsigned int width, unsigned int height) {
  mFramebuffer.resize(width, height);
  glViewport(0, 0, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData) {
  Logger::log(1, "%s: OpenGL Render uploadData \n", __FUNCTION__);
  mRenderData.rdTriangleCount = vertexData.vertices.size();
  mVertexBuffer.uploadData(vertexData);
}

void OGLRenderer::draw() {
  Logger::log(1, "%s: OpenGL Render draw \n", __FUNCTION__);

  // TODO add in the upload to VBO timer

  /* upload required data only when switching GPU and CPU */
  static bool lastGPURenderState = mRenderData.rdGPUVertexSkinning;

  if (lastGPURenderState != mRenderData.rdGPUVertexSkinning) {
    mModelUploadRequired = true;
    lastGPURenderState = mRenderData.rdGPUVertexSkinning;
  }
  if (mModelUploadRequired) {
    mGltfModel->uploadVertexBuffers();
    mModelUploadRequired = false;
  }

  if (!mRenderData.rdGPUVertexSkinning) {
    /* glTF vertex skinning, overwrites position buffer, needs upload on every frame */
    mGltfModel->applyCPUVertexSkinning();
  }

  double tickTime = glfwGetTime();
  mRenderData.rdTickDiff = tickTime - lastTickTime;

  static float prevFrameStartTime = 0.0f;
  float frameStartTime = glfwGetTime();

  handleMovementKeys();

  // Bind buffer to let us receive vertex data.
  mFramebuffer.bind();
  glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);

  mProjectionMatrix = glm::perspective(glm::radians(static_cast<float>(mRenderData.rdFieldOfView)),
                                       static_cast<float>(mRenderData.rdWidth) /
                                           static_cast<float>(mRenderData.rdHeight),
                                       0.1f,
                                       100.f);

  mViewMatrix = mCamera.getViewMatrix(mRenderData);



  /* Animate */
  mRenderData.rdClipName = mGltfModel->getClipName(mRenderData.rdAnimClip);
  if (mRenderData.rdPlayAnimation) {
    std::cout << "play animation true" << std::endl;
    mGltfModel->playAnimation(mRenderData.rdAnimClip, mRenderData.rdAnimSpeed);
  }
  else {
    mRenderData.rdAnimEndTime = mGltfModel->getAnimationEndTime(mRenderData.rdAnimClip);
    mGltfModel->setAnimationFrame(mRenderData.rdAnimClip, mRenderData.rdAnimTimePosition);
  }

    std::cout << "made it here" << std::endl;

  std::vector<glm::mat4> matrixData;
  matrixData.push_back(mViewMatrix);
  matrixData.push_back(mProjectionMatrix);
  mUniformBuffer.uploadUboData(matrixData, 0);

  if (mRenderData.rdGPUVertexSkinning) {
    mGltfShaderStorageBuffer.uploadSsboData(mGltfModel->getJointMatrices(), 1);
  }

  mTex.bind();
  mVertexBuffer.bind();
  mVertexBuffer.draw(GL_TRIANGLES, 0, mRenderData.rdTriangleCount);
  mVertexBuffer.unbind();
  mTex.unbind();

  /* draw the glTF model */
  if (mRenderData.rdGPUVertexSkinning) {
    mGltfGPUShader.use();
  }
  else {
    mGltfShader.use();
  }

  mGltfModel->draw();

  mFramebuffer.unbind();
  mFramebuffer.drawToScreen();

  mUIGenerateTimer.start();
  mUserInterface.createFrame(mRenderData);
  mRenderData.rdUIGenerateTime = mUIGenerateTimer.stop();
  mUserInterface.render();

  //  Calculate the FPS
  mRenderData.rdFrameTime = frameStartTime - prevFrameStartTime;
  prevFrameStartTime = frameStartTime;
  lastTickTime = tickTime;
}

void OGLRenderer::cleanup() {
  mUserInterface.cleanup();

  mBasicShader.cleanup();
  mChangedShader.cleanup();
  mGltfShader.cleanup();
  mGltfGPUShader.cleanup();

  mTex.cleanup();
  mGltfModel->cleanup();
  mGltfModel.reset();
  mVertexBuffer.cleanup();
  mFramebuffer.cleanup();
}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods) {
  Logger::log(1, "%s: Render key handle event \n", __FUNCTION__);
}

/* Mouse Handlers. */
void OGLRenderer::handleMouseButtonEvents(int button, int action, int mods) {
  ImGuiIO &io = ImGui::GetIO();
  if (button >= 0 && button < ImGuiMouseButton_COUNT) {
    io.AddMouseButtonEvent(button, action == GLFW_PRESS);
  }
  // Check if ImGui is handling a mouse event
  if (io.WantCaptureMouse) {
    return;
  }

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    mMouseLock = !mMouseLock;

    if (mMouseLock) {
      glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      /* enable raw mode if possible */
      if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(mRenderData.rdWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
      }
    }
    else {
      glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
}

void OGLRenderer::handleMousePositionEvents(double xPos, double yPos) {

  /* forward to ImGui */
  ImGuiIO &io = ImGui::GetIO();
  io.AddMousePosEvent((float)xPos, (float)yPos);

  /* hide from application */
  if (io.WantCaptureMouse) {
    return;
  }

  /* calculate relative movement from last position */
  int mouseMoveRelX = static_cast<int>(xPos) - mMouseXPos;
  int mouseMoveRelY = static_cast<int>(yPos) - mMouseYPos;

  if (mMouseLock) {
    mRenderData.rdViewAzimuth += mouseMoveRelX / 10.0;

    // Add 360 degree limits to Azimuth
    if (mRenderData.rdViewAzimuth < 0.0) {
      mRenderData.rdViewAzimuth += 360.0;
    }
    if (mRenderData.rdViewAzimuth >= 360.0) {
      mRenderData.rdViewAzimuth -= 360.0;
    }

    mRenderData.rdViewElevation -= mouseMoveRelY / 10.0;

    // Add (-89, 89) degree limits to Elevation
    if (mRenderData.rdViewElevation > 89.0) {
      mRenderData.rdViewElevation = 89.0;
    }
    if (mRenderData.rdViewElevation < -89.0) {
      mRenderData.rdViewElevation = -89.0;
    }
  }

  /* save old values*/
  mMouseXPos = static_cast<int>(xPos);
  mMouseYPos = static_cast<int>(yPos);
}

void OGLRenderer::handleMovementKeys() {
  mRenderData.rdMoveForward = 0;

  if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_W) == GLFW_PRESS) {
    mRenderData.rdMoveForward += 1;
  }
  if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_S) == GLFW_PRESS) {
    mRenderData.rdMoveForward -= 1;
  }

  mRenderData.rdMoveRight = 0;

  if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_D) == GLFW_PRESS) {
    mRenderData.rdMoveRight += 1;
  }
  if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_A) == GLFW_PRESS) {
    mRenderData.rdMoveRight -= 1;
  }

  mRenderData.rdMoveUp = 0;

  if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_E) == GLFW_PRESS) {
    mRenderData.rdMoveUp += 1;
  }
  if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_Q) == GLFW_PRESS) {
    mRenderData.rdMoveUp -= 1;
  }
}