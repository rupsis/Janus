#include "Logger.h"
#include "Window.h"
#include <memory>

int main(int arcc, char *argc[]) {
  std::unique_ptr<Window> w = std::make_unique<Window>();

  if (!w->init(640, 480, "Test Window")) {
    Logger::log(1, "%s error: Window init error\n", __FUNCTION__);
    return -1;
  }
  w->mainLoop();
  w->cleanup();
  return 0;
}