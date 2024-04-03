#pragma once
#include "OGLRenderData.h"
#include <vector>

class UserInterface {
 public:
  void init(OGLRenderData &renderData);
  void createFrame(OGLRenderData &renderData);
  void render();
  void cleanup();

 private:
  float mFramesPerSecond = 0.0f;
  float mAveragingAlpha = 0.96f;

  std::vector<float> mFPSValues{};
  int mNumFPSValues = 90;
};