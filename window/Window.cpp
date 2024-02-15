#include "Window.h"
#include "Logger.h"

bool Window::init(unsigned int width, unsigned int height, std::string title) {
  // Check to see if glfw inits on our system
  if (!glfwInit()) {
    Logger::log(1, "%s: glfwInit() error\n", __FUNCTION__);
    return false;
  }

  // Set a 'hint' for the NEXT window created.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

  if (!mWindow) {
    Logger::log(1, "%s: Could not create window\n", __FUNCTION__);
    glfwTerminate();
    return false;
  }

  // /* 1) save the pointer to the instance as user pointer */
  // glfwSetWindowUserPointer(mWindow, this);

  // /* 2) use a lambda to get the pointer and call the member function */
  // glfwSetWindowCloseCallback(mWindow, [](GLFWwindow *win) {
  //   auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(win));
  //   thisWindow->handleWindowCloseEvents();
  // });

  // glfwSetWindowPosCallback(mWindow, [](GLFWwindow *win, int xpos, int ypos) {
  //   auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(win));
  //   thisWindow->handleWindowMoveEvents(xpos, ypos);
  // });

  // glfwSetKeyCallback(mWindow, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
  //   auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(win));
  //   thisWindow->handleKeyEvents(key, scancode, action, mods);
  // });

  // glfwSetMouseButtonCallback(mWindow, [](GLFWwindow *win, int button, int action, int mods) {
  //   auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(win));
  //   thisWindow->handleMouseButtonEvents(button, action, mods);
  // });

  glfwMakeContextCurrent(mWindow);

  mRenderer = std::make_unique<OGLRenderer>();
  if (!mRenderer->init(width, height)) {
    glfwTerminate();
    return false;
  }

  glfwSetWindowUserPointer(mWindow, mRenderer.get());
  glfwSetWindowSizeCallback(mWindow, [](GLFWwindow *win, int width, int height) {
    auto renderer = static_cast<OGLRenderer *>(glfwGetWindowUserPointer(win));
    renderer->setSize(width, height);
  });

  mModel = std::make_unique<Model>();
  mModel->init();

  Logger::log(1, "%s: Window successfully initialized\n", __FUNCTION__);

  return true;
}

void Window::mainLoop() {
  // Wait for vertical sync, else flickering / tearing might occur.
  glfwSwapInterval(1);
  float color = 0.0f;

  mRenderer->uploadData(mModel->getVertexData());

  while (!glfwWindowShouldClose(mWindow)) {
    mRenderer->draw();
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
  }
}

void Window::cleanup() {
  Logger::log(1, "%s: Terminating Window\n", __FUNCTION__);
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

/** Window event handlers. */
void Window::handleWindowCloseEvents() {
  Logger::log(1, "%s: Window got close event... bye!\n", __FUNCTION__);
}

void Window::handleWindowMoveEvents(int xpos, int ypos) {
  Logger::log(1, "%s: Window updated to (%i,%i)\n", __FUNCTION__, xpos, ypos);
}

/** Keyboard event handler. */
void Window::handleKeyEvents(int key, int scancode, int action, int mods) {
  std::string actionName;

  switch (action) {
    case GLFW_PRESS:
      actionName = "pressed";
      break;
    case GLFW_RELEASE:
      actionName = "released";
      break;
    case GLFW_REPEAT:
      actionName = "repeated";
      break;
    default:
      actionName = "invalid";
      break;
  }

  const char *keyName = glfwGetKeyName(key, 0);
  Logger::log(1,
              "%s: key %s (key %i, scancode %i) %s\n",
              __FUNCTION__,
              keyName,
              key,
              scancode,
              actionName.c_str());
}

/** Mouse button events handlers. */
void Window::handleMouseButtonEvents(int button, int action, int mods) {
  std::string actionName;

  switch (action) {
    case GLFW_PRESS:
      actionName = "pressed";
      break;
    case GLFW_RELEASE:
      actionName = "released";
      break;
    default:
      actionName = "invalid";
      break;
  }

  std::string mouseButtonName;
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      mouseButtonName = "left";
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      mouseButtonName = "middle";
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      mouseButtonName = "right";
      break;
    default:
      mouseButtonName = "other";
      break;
  }

  Logger::log(1,
              "%s: %s mouse button (%i) %s\n",
              __FUNCTION__,
              mouseButtonName.c_str(),
              button,
              actionName.c_str());
}
