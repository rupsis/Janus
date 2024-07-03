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
  void renderInfo(OGLRenderData &renderData);
  void renderFPSGraph(OGLRenderData &renderData);
  void renderTimers(OGLRenderData &renderData);
  void renderCamera(OGLRenderData &renderData);
  void renderModelControls(OGLRenderData &renderData);
  void renderAnimationControls(OGLRenderData &renderData);
  void renderAnimationBlendingControls(OGLRenderData &renderData);
  void renderIKControls(OGLRenderData &renderData);

  float mFramesPerSecond = 0.0f;
  float mAveragingAlpha = 0.96f;

  /* UI circular buffers for time series.*/
  std::vector<float> mFPSValues{};
  int mNumFPSValues = 90;

  std::vector<float> mFrameTimeValues{};
  int mNumFrameTimeValues = 90;

  std::vector<float> mModelUploadValues{};
  int mNumModelUploadValues = 90;

  std::vector<float> mMatrixGenerationValues{};
  int mNumMatrixGenerationValues = 90;

  std::vector<float> mIKValues{};
  int mNumIKValues = 90;

  std::vector<float> mMatrixUploadValues{};
  int mNumMatrixUploadValues = 90;

  std::vector<float> mUiGenValues{};
  int mNumUiGenValues = 90;

  std::vector<float> mUiDrawValues{};
  int mNumUiDrawValues = 90;
};