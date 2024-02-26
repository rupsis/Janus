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

  ImGui::Text("Triangles:");
  ImGui::SameLine();
  ImGui::Text(std::to_string(renderData.rdTirangleCount).c_str());

  std::string windowDims = std::to_string(renderData.rdHeight) + "x" +
                           std::to_string(renderData.rdWidth);
  ImGui::Text("Window Dimensions:");
  ImGui::SameLine();
  ImGui::Text(windowDims.c_str());

  std::string imgWindowPos = std::to_string(static_cast<int>(ImGui::GetWindowPos().x)) + "/" +
                             std::to_string(static_cast<int>(ImGui::GetWindowPos().y));
  ImGui::Text("ImGui Window Position:");
  ImGui::SameLine();
  ImGui::Text(imgWindowPos.c_str());

  ImGui::End();
}

void UserInterface::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}