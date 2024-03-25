#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <string>
#include <tiny_gltf.h>
#include <vector>

enum class ETargetPath { ROTATION, TRANSLATION, SCALE };

enum class EInterpolationType { STEP, LINEAR, CUBICSPLINE };

class GltfAnimationChannel {
 public:
  void loadChannelData(std::shared_ptr<tinygltf::Model> model,
                       tinygltf::Animation anim,
                       tinygltf::AnimationChannel channel);
  int getTargetNode();
  ETargetPath getTargetPath();

  glm::vec3 getScaling(float time);
  glm::vec3 getTranslation(float time);
  glm::quat getRotation(float time);

  float getMaxTime();

 private:
  int mTargetNode = -1;
  ETargetPath mTargetPath = ETargetPath::ROTATION;
  EInterpolationType mInterType = EInterpolationType::LINEAR;

  std::vector<float> mTimings{};
  std::vector<glm::vec3> mScaling{};
  std::vector<glm::vec3> mTranslations{};
  std::vector<glm::quat> mRotations{};

  void setTimings(std::vector<float> timings);
  void setScalings(std::vector<glm::vec3> scalings);
  void setTranslations(std::vector<glm::vec3> translations);
  void setRotations(std::vector<glm::quat> rotations);

  // Helper methods
  float interpolatedTime(float time, int prevTimeIndex, int nextTimeIndex);
};