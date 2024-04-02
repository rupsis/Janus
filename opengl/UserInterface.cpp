#include "UserInterface.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>

/* UI forward declaration methods. */
static void renderInfo(OGLRenderData &renderData);
static void renderTimers(OGLRenderData &renderData);
static void renderCamera(OGLRenderData &renderData);
static void renderModelControls(OGLRenderData &renderData);
static void renderAnimationControls(OGLRenderData &renderData);
static void renderAnimationBlendingControls(OGLRenderData &renderData);

void UserInterface::init(OGLRenderData &renderData) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(renderData.rdWindow, true);
  const char *glslVersion = "#version 460 core";
  ImGui_ImplOpenGL3_Init(glslVersion);
  ImGui::StyleColorsDark();
}

void UserInterface::cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UserInterface::createFrame(OGLRenderData &renderData) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiWindowFlags imguiWindowFlags = 0;
  ImGui::SetNextWindowBgAlpha(0.8f);
  ImGui::Begin("Control", nullptr, imguiWindowFlags);

  static float newFps = 0.0f;
  // Avoid division by 0
  if (renderData.rdFrameTime > 0.0) {
    newFps = 1.0f / renderData.rdFrameTime;
  }
  //  Average out previous FPS with new FPS based on averaging Alpha
  framesPerSecond = (averagingAlpha * framesPerSecond) + (1.0f - averagingAlpha) * newFps;

  ImGui::Text("FPS:");
  ImGui::SameLine();
  ImGui::Text("%s", std::to_string(framesPerSecond).c_str());
  ImGui::Separator();

  renderInfo(renderData);

  renderTimers(renderData);

  renderCamera(renderData);

  renderModelControls(renderData);

  renderAnimationControls(renderData);

  ImGui::End();
}

void UserInterface::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* UI Sections. */
static void renderInfo(OGLRenderData &renderData) {
  if (ImGui::CollapsingHeader("Info")) {
    ImGui::Text("Triangles:");
    ImGui::SameLine();
    ImGui::Text(
        "%s", std::to_string(renderData.rdTriangleCount + renderData.rdGltfTriangleCount).c_str());

    std::string windowDims = std::to_string(renderData.rdHeight) + "x" +
                             std::to_string(renderData.rdWidth);
    ImGui::Text("Window Dimensions:");
    ImGui::SameLine();
    ImGui::Text("%s", windowDims.c_str());

    std::string imgWindowPos = std::to_string(static_cast<int>(ImGui::GetWindowPos().x)) + "/" +
                               std::to_string(static_cast<int>(ImGui::GetWindowPos().y));
    ImGui::Text("ImGui Window Position:");
    ImGui::SameLine();
    ImGui::Text("%s", imgWindowPos.c_str());
  }
}

static void renderTimers(OGLRenderData &renderData) {
  if (ImGui::CollapsingHeader("Timers")) {
    ImGui::Text("Frame Time:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdFrameTime).c_str());
    ImGui::SameLine();
    ImGui::Text("ms");

    ImGui::Text("Model Upload Time:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdUploadToVBOTime).c_str());
    ImGui::SameLine();
    ImGui::Text("ms");

    ImGui::Text("Matrix Generation Time:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdMatrixGenerateTime).c_str());
    ImGui::SameLine();
    ImGui::Text("ms");

    ImGui::Text("Matrix Upload Time:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdUploadToUBOTime).c_str());
    ImGui::SameLine();
    ImGui::Text("ms");

    ImGui::Text("UI Generation Time:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdUIGenerateTime).c_str());
    ImGui::SameLine();
    ImGui::Text("ms");

    ImGui::Text("UI Draw Time:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdUIDrawTime).c_str());
    ImGui::SameLine();
    ImGui::Text("ms");
  }
}

static void renderCamera(OGLRenderData &renderData) {
  if (ImGui::CollapsingHeader("Camera")) {
    ImGui::Text("Camera Position:");
    ImGui::SameLine();
    ImGui::Text("%s", glm::to_string(renderData.rdCameraWorldPosition).c_str());

    ImGui::Text("View Azimuth:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdViewAzimuth).c_str());
    ImGui::Text("View Elevation:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdViewElevation).c_str());
    ImGui::Separator();

    ImGui::Text("Field of View");
    ImGui::SameLine();
    ImGui::SliderInt("##FOV", &renderData.rdFieldOfView, 40, 150);
  }
}

static void renderModelControls(OGLRenderData &renderData) {
  ImGui::Checkbox("Draw Model", &renderData.rdDrawGltfModel);
  ImGui::Checkbox("Draw Skeleton", &renderData.rdDrawSkeleton);

  ImGui::Checkbox("GPU Vertex Skinning Method:", &renderData.rdGPUDualQuatVertexSkinning);
  ImGui::SameLine();
  if (renderData.rdGPUDualQuatVertexSkinning) {
    ImGui::Text("Dual Quaternion");
  }
  else {
    ImGui::Text("Linear");
  }
}

static void renderAnimationControls(OGLRenderData &renderData) {
  if (ImGui::CollapsingHeader("glTF Animation")) {
    ImGui::Text("Clip No");
    ImGui::SameLine();
    ImGui::SliderInt("##Clip", &renderData.rdAnimClip, 0, renderData.rdAnimClipSize - 1);
    ImGui::Text("Clip Name: %s", renderData.rdClipName.c_str());
    ImGui::Checkbox("Play Animation", &renderData.rdPlayAnimation);

    renderAnimationBlendingControls(renderData);

    if (!renderData.rdPlayAnimation) {
      ImGui::BeginDisabled();
    }
    ImGui::Text("Speed ");
    ImGui::SameLine();
    ImGui::SliderFloat("##ClipSpeed", &renderData.rdAnimSpeed, 0.0f, 2.0f);
    if (!renderData.rdPlayAnimation) {
      ImGui::EndDisabled();
    }

    if (renderData.rdPlayAnimation) {
      ImGui::BeginDisabled();
    }
    ImGui::Text("Timepos");
    ImGui::SameLine();
    ImGui::SliderFloat(
        "##ClipPos", &renderData.rdAnimTimePosition, 0.0f, renderData.rdAnimEndTime);
    if (renderData.rdPlayAnimation) {
      ImGui::EndDisabled();
    }
  }
}

static void renderAnimationBlendingControls(OGLRenderData &renderData) {
  if (ImGui::CollapsingHeader("glTF Animation Blending")) {
    ImGui::Checkbox("Blending Type:", &renderData.rdCrossBlending);
    ImGui::SameLine();
    if (renderData.rdCrossBlending) {
      ImGui::Text("Cross");
    }
    else {
      ImGui::Text("Single");
    }

    if (renderData.rdCrossBlending) {
      ImGui::BeginDisabled();
    }
    ImGui::Text("Blend Factor");
    ImGui::SameLine();
    ImGui::SliderFloat("##BlendFactor", &renderData.rdAnimBlendFactor, 0.0f, 1.0f);
    if (renderData.rdCrossBlending) {
      ImGui::EndDisabled();
    }
  }

  if (!renderData.rdCrossBlending) {
    ImGui::BeginDisabled();
  }
  ImGui::Text("Dest Clip   ");
  ImGui::SameLine();
  ImGui::SliderInt(
      "##DestClip", &renderData.rdCrossBlendDestAnimClip, 0, renderData.rdAnimClipSize - 1);

  ImGui::Text("Dest Clip Name: %s", renderData.rdCrossBlendDestClipName.c_str());
  ImGui::SameLine();
  ImGui::SliderFloat("##CrossBlendFactor", &renderData.rdAnimCrossBlendFactor, 0.0f, 1.0f);

  if (!renderData.rdCrossBlending) {
    ImGui::EndDisabled();
  }
}