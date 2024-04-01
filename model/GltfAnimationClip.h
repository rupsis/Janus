#pragma once
#include "GltfAnimationChannel.h"
#include "GltfNode.h"
#include <memory>
#include <string>
#include <tiny_gltf.h>
#include <vector>

class GltfAnimationClip {
 public:
  GltfAnimationClip(std::string name);
  void addChannel(std::shared_ptr<tinygltf::Model> model,
                  tinygltf::Animation anim,
                  tinygltf::AnimationChannel channel);

  void setAnimationFrame(std::vector<std::shared_ptr<GltfNode>> nodes, float time);
  float getClipEndTime();
  std::string getClipName();

 private:
  std::vector<std::shared_ptr<GltfAnimationChannel>> mAnimationChannels;
  std::string mClipName;
};
