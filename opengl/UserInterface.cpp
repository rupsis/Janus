#include "UserInterface.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>

void UserInterface::init(OGLRenderData &renderData) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(renderData.rdWindow, true);
  const char *glslVersion = "#version 460 core";
  ImGui_ImplOpenGL3_Init(glslVersion);
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

  // TODO make it's own method?
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

  ImGui::Text("UI Generation Time:");
  ImGui::SameLine();
  ImGui::Text("%s", std::to_string(renderData.rdUIGenerateTime).c_str());
  ImGui::SameLine();
  ImGui::Text("ms");
  ImGui::Separator();

  ImGui::Text("Triangles:");
  ImGui::SameLine();
  ImGui::Text("%s", std::to_string(renderData.rdTriangleCount).c_str());

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

  static bool checkBoxChecked = false;
  ImGui::Checkbox("Check Me", &checkBoxChecked);

  if (checkBoxChecked) {
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    ImGui::Text("Yes");
    ImGui::PopStyleColor();
  }

  if (ImGui::Button("Toggle Shader")) {
    renderData.rdUseChangedShader = !renderData.rdUseChangedShader;
  }

  ImGui::SameLine();
  if (!renderData.rdUseChangedShader) {
    ImGui::Text("Basic Shader");
  }
  else {
    ImGui::Text("Changed Shader");
  }

  ImGui::End();
}

void UserInterface::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}