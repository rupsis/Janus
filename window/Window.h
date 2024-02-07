#pragma once
#include <GLFW/glfw3.h>
#include <string>

class Window {
 public:
  bool init(unsigned int width, unsigned int height, std::string title);
  void mainLoop();
  void cleanup();

  /** Keyboard event handler. */
  void handleKeyEvents(int key, int scancode, int action, int mods);

 private:
  GLFWwindow *mWindow = nullptr;

  /** Window event handlers. */
  void handleWindowCloseEvents();
  void handleWindowMoveEvents(int xpos, int ypos);

  /** Mouse event handler.*/
  void handleMouseButtonEvents(int button, int action, int mods);
};