#include "GltfAnimationClip.h"

GltfAnimationClip::GltfAnimationClip(std::string name) : mClipName(name) {}

void GltfAnimationClip::addChannel(std::shared_ptr<tinygltf::Model> model,
                                   tinygltf::Animation anim,
                                   tinygltf::AnimationChannel channel) {
  std::shared_ptr<GltfAnimationChannel> chan = std::make_shared<GltfAnimationChannel>();
  chan->loadChannelData(model, anim, channel);
  mAnimationChannels.push_back(chan);
}

void GltfAnimationClip::setAnimationFrame(std::vector<std::shared_ptr<GltfNode>> nodes,
                                          float time) {
  // Loop through all challen in the clip and update specifed property
  for (auto &channel : mAnimationChannels) {
    int targetNode = channel->getTargetNode();

    switch (channel->getTargetPath()) {
      case ETargetPath::ROTATION:
        nodes.at(targetNode)->setRotation(channel->getRotation(time));
        break;
      case ETargetPath::TRANSLATION:
        nodes.at(targetNode)->setTranslation(channel->getTranslation(time));
        break;
      case ETargetPath::SCALE:
        nodes.at(targetNode)->setScale(channel->getScaling(time));
        break;
    }
  }

  // once all properties are set, update trans/rot/scale matrices
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
