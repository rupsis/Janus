#include "GltfAnimationClip.h"

#include <iostream>

GltfAnimationClip::GltfAnimationClip(std::string name) : mClipName(name) {}

void GltfAnimationClip::addChannel(std::shared_ptr<tinygltf::Model> model,
                                   tinygltf::Animation anim,
                                   tinygltf::AnimationChannel channel) {
  std::shared_ptr<GltfAnimationChannel> chan = std::make_shared<GltfAnimationChannel>();
  chan->loadChannelData(model, anim, channel);
  mAnimationChannels.push_back(chan);
}

void GltfAnimationClip::blendAnimationFrame(std::vector<std::shared_ptr<GltfNode>> nodes,
                                            float time,
                                            float blendFactor) {
  for (auto &channel : mAnimationChannels) {
    int targetNode = channel->getTargetNode();
    switch (channel->getTargetPath()) {
      case ETargetPath::ROTATION:
        nodes.at(targetNode)->blendRotation(channel->getRotation(time), blendFactor);
        break;
      case ETargetPath::TRANSLATION:
        nodes.at(targetNode)->blendRotation(channel->getTranslation(time), blendFactor);
        break;
      case ETargetPath::SCALE:
        nodes.at(targetNode)->blendScale(channel->getScaling(time), blendFactor);
        break;
    }
  }
  /* update all nodes in a single run */
  for (auto &node : nodes) {
    if (node) {
      node->calculateLocalTRSMatrix();
    }
  }
}

float GltfAnimationClip::getClipEndTime() {
  return mAnimationChannels.at(0)->getMaxTime();
}

std::string GltfAnimationClip::getClipName() {
  return mClipName;
}
