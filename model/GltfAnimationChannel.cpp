#include "GltfAnimationChannel.h"

#include <iostream>

void GltfAnimationChannel::loadChannelData(std::shared_ptr<tinygltf::Model> model,
                                           tinygltf::Animation anim,
                                           tinygltf::AnimationChannel channel) {
  mTargetNode = channel.target_node;

  // extract input data from GLTF file
  const tinygltf::Accessor& inputAccessor = model->accessors.at(anim.samplers.at(channel.sampler).input);
  const tinygltf::BufferView &inputBufferView = model->bufferViews.at(inputAccessor.bufferView);
  const tinygltf::Buffer &inputBuffer = model->buffers.at(inputBufferView.buffer);

   // Allocate memory for raw time values
  std::vector<float> timings;
  timings.resize(inputAccessor.count);

 
  std::memcpy(timings.data(),
              &inputBuffer.data.at(0) + inputBufferView.byteOffset,
              inputBufferView.byteLength);
  setTimings(timings);

  const tinygltf::AnimationSampler sampler = anim.samplers.at(channel.sampler);

  if (sampler.interpolation.compare("STEP") == 0) {
    mInterType = EInterpolationType::STEP;
  }
  else if (sampler.interpolation.compare("LINEAR") == 0) {
    mInterType = EInterpolationType::LINEAR;
  }
  else {
    mInterType = EInterpolationType::CUBICSPLINE;
  }

  // Extract output data from GLTF file
  const tinygltf::Accessor &outputAccessor = model->accessors.at(
      anim.samplers.at(channel.sampler).output);
  const tinygltf::BufferView &outputBufferView = model->bufferViews.at(outputAccessor.bufferView);
  const tinygltf::Buffer &outputBuffer = model->buffers.at(outputBufferView.buffer);

  if (channel.target_path.compare("rotation") == 0) {
    mTargetPath = ETargetPath::ROTATION;
    std::vector<glm::quat> rotations;
    rotations.resize(outputAccessor.count);

    std::memcpy(rotations.data(),
                &outputBuffer.data.at(0) + outputBufferView.byteOffset,
                outputBufferView.byteLength);
    setRotations(rotations);
  }
  else if (channel.target_path.compare("translation") == 0) {
    mTargetPath = ETargetPath::TRANSLATION;
    std::vector<glm::vec3> translations;
    translations.resize(outputAccessor.count);

    std::memcpy(translations.data(),
                &outputBuffer.data.at(0) + outputBufferView.byteOffset,
                outputBufferView.byteLength);
    setTranslations(translations);
  }
  else {
    mTargetPath = ETargetPath::SCALE;
    std::vector<glm::vec3> scale;
    scale.resize(outputAccessor.count);

    std::memcpy(scale.data(),
                &outputBuffer.data.at(0) + outputBufferView.byteOffset,
                outputBufferView.byteLength);
    setScalings(scale);
  }
  // TODO add morph targets?
}

float GltfAnimationChannel::calculateInterpolatedTime(float time, int prevTimeIndex, int nextTimeIndex) {
  return (time - mTimings.at(prevTimeIndex)) /
         (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));
}

/* Getters */

float GltfAnimationChannel::getMaxTime() {
  return mTimings.at(mTimings.size() - 1);
}

glm::vec3 GltfAnimationChannel::getScaling(float time) {
  std::cout << "getting scale channel" << std::endl;
  if (mScaling.size() == 0) {
    return glm::vec3(1.0f);
  }

  // if time is before our first timings point, return first point.
  if (time < mTimings.at(0)) {
    return mScaling.at(0);
  }

  // if time is after the timings point, return last point.
  if (time > mTimings.at(mTimings.size() - 1)) {
    return mScaling.at(mScaling.size() - 1);
  }

  int prevTimeIndex = 0;
  int nextTimeIndex = 0;
  // get two indicies, one before, and one after given time
  for (int i = 0; i < mTimings.size(); ++i) {
    if (mTimings.at(i) > time) {
      nextTimeIndex = i;
      break;
    }
    prevTimeIndex = i;
  }

  // if at last point in time, return that value
  if (prevTimeIndex == nextTimeIndex) {
    return mScaling.at(prevTimeIndex);
  }

  glm::vec3 finalScale = glm::vec3(1.0f);

  switch (mInterType) {
    case EInterpolationType::STEP:
      finalScale = mScaling.at(prevTimeIndex);
      break;
    case EInterpolationType::LINEAR: {
      float interpolatedTime = calculateInterpolatedTime(time, prevTimeIndex, nextTimeIndex);
      glm::vec3 prevScale = mScaling.at(prevTimeIndex);
      glm::vec3 nextScale = mScaling.at(nextTimeIndex);
      finalScale = prevScale + interpolatedTime * (nextScale - prevScale);
    } break;
    case EInterpolationType::CUBICSPLINE: {
      float deltaTime = mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex);
      //   Tangents are stored in 3, in-tangent, data value, out-tangent
      glm::vec3 prevTangent = deltaTime * mScaling.at(prevTimeIndex * 3 + 2);
      glm::vec3 nextTangent = deltaTime * mScaling.at(nextTimeIndex * 3);

      float interpolatedTime = calculateInterpolatedTime(time, prevTimeIndex, nextTimeIndex);

      float interpolatedTimeSq = interpolatedTime * interpolatedTime;
      float interpolatedTimeCub = interpolatedTimeSq * interpolatedTime;

      glm::vec3 prevPoint = mScaling.at(prevTimeIndex * 3 + 1);
      glm::vec3 nextPoint = mScaling.at(nextTimeIndex * 3 + 1);

      // good luck!
      finalScale = (2 * interpolatedTimeCub - 3 * interpolatedTimeSq + 1) * prevPoint +
                   (interpolatedTimeCub - 2 * interpolatedTimeSq + interpolatedTime) *
                       prevTangent +
                   (-2 * interpolatedTimeCub + 3 * interpolatedTimeSq) * nextPoint +
                   (interpolatedTimeCub - interpolatedTimeSq) * nextTangent;
    } break;
  }
  return finalScale;
}

glm::vec3 GltfAnimationChannel::getTranslation(float time) {
  std::cout << "getting translation channel" << std::endl;
  if (mTranslations.size() == 0) {
    return glm::vec3(0.0f);
  }

  // if time is before our first timings point, return first point.
  if (time < mTimings.at(0)) {
    return mTranslations.at(0);
  }

  // if time is after the timings point, return last point.
  if (time > mTimings.at(mTimings.size() - 1)) {
    return mTranslations.at(mTranslations.size() - 1);
  }

  int prevTimeIndex = 0;
  int nextTimeIndex = 0;
  // get two indicies, one before, and one after given time
  for (int i = 0; i < mTimings.size(); ++i) {
    if (mTimings.at(i) > time) {
      nextTimeIndex = i;
      break;
    }
    prevTimeIndex = i;
  }

  // if at last point in time, return that value
  if (prevTimeIndex == nextTimeIndex) {
    return mTranslations.at(prevTimeIndex);
  }

  glm::vec3 finalTranslation = glm::vec3(0.0f);

  switch (mInterType) {
    case EInterpolationType::STEP:
      finalTranslation = mTranslations.at(prevTimeIndex);
      break;
    case EInterpolationType::LINEAR: {
      float interpolatedTime = (time - mTimings.at(prevTimeIndex)) /
                               (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));
      glm::vec3 prevTranslate = mTranslations.at(prevTimeIndex);
      glm::vec3 nextTranslate = mTranslations.at(nextTimeIndex);
      finalTranslation = prevTranslate + interpolatedTime * (nextTranslate - prevTranslate);
    } break;
    case EInterpolationType::CUBICSPLINE: {
      float deltaTime = mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex);
      //   Tangents are stored in 3, in-tangent, data value, out-tangent
      glm::vec3 prevTangent = deltaTime * mTranslations.at(prevTimeIndex * 3 + 2);
      glm::vec3 nextTangent = deltaTime * mTranslations.at(nextTimeIndex * 3);

      float interpolatedTime = (time - mTimings.at(prevTimeIndex)) /
                               (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));

      float interpolatedTimeSq = interpolatedTime * interpolatedTime;
      float interpolatedTimeCub = interpolatedTimeSq * interpolatedTime;

      glm::vec3 prevPoint = mTranslations.at(prevTimeIndex * 3 + 1);
      glm::vec3 nextPoint = mTranslations.at(nextTimeIndex * 3 + 1);

      finalTranslation = (2 * interpolatedTimeCub - 3 * interpolatedTimeSq + 1) * prevPoint +
                         (interpolatedTimeCub - 2 * interpolatedTimeSq + interpolatedTime) *
                             prevTangent +
                         (-2 * interpolatedTimeCub + 3 * interpolatedTimeSq) * nextPoint +
                         (interpolatedTimeCub - interpolatedTimeSq) * nextTangent;
    } break;
  }
  return finalTranslation;
}

glm::quat GltfAnimationChannel::getRotation(float time) {

  if (mRotations.size() == 0) {
    return glm::identity<glm::quat>();
  }


  // if time is before our first timings point, return first point.
  if (time < mTimings.at(0)) {
    return mRotations.at(0);
  }

  // if time is after the timings point, return last point.
  if (time > mTimings.at(mTimings.size() - 1)) {
    return mRotations.at(mRotations.size() - 1);
  }

    
  int prevTimeIndex = 0;
  int nextTimeIndex = 0;
  // get two indicies, one before, and one after given time
  for (int i = 0; i < mTimings.size(); ++i) {
    if (mTimings.at(i) > time) {
      nextTimeIndex = i;
      break;
    }
    prevTimeIndex = i;
  }


  // if at last point in time, return that value
  if (prevTimeIndex == nextTimeIndex) {
    return mRotations.at(prevTimeIndex);
  }

  

  glm::quat finalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  switch (mInterType) {
    case EInterpolationType::STEP:
      finalRotation = mRotations.at(prevTimeIndex);
      break;
    case EInterpolationType::LINEAR: {
      float interpolatedTime = (time - mTimings.at(prevTimeIndex)) /
                               (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));
      glm::quat prevRotate = mRotations.at(prevTimeIndex);
      glm::quat nextRotate = mRotations.at(nextTimeIndex);
      finalRotation = prevRotate + interpolatedTime * (nextRotate - prevRotate);
    } break;
    case EInterpolationType::CUBICSPLINE: {
      float deltaTime = mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex);
      //   Tangents are stored in 3, in-tangent, data value, out-tangent
      glm::quat prevTangent = deltaTime * mRotations.at(prevTimeIndex * 3 + 2);
      glm::quat nextTangent = deltaTime * mRotations.at(nextTimeIndex * 3);

      float interpolatedTime = (time - mTimings.at(prevTimeIndex)) /
                               (mTimings.at(nextTimeIndex) - mTimings.at(prevTimeIndex));

      float interpolatedTimeSq = interpolatedTime * interpolatedTime;
      float interpolatedTimeCub = interpolatedTimeSq * interpolatedTime;

      glm::quat prevPoint = mRotations.at(prevTimeIndex * 3 + 1);
      glm::quat nextPoint = mRotations.at(nextTimeIndex * 3 + 1);

      finalRotation = (2 * interpolatedTimeCub - 3 * interpolatedTimeSq + 1) * prevPoint +
                      (interpolatedTimeCub - 2 * interpolatedTimeSq + interpolatedTime) *
                          prevTangent +
                      (-2 * interpolatedTimeCub + 3 * interpolatedTimeSq) * nextPoint +
                      (interpolatedTimeCub - interpolatedTimeSq) * nextTangent;
    } break;
  }
  return finalRotation;
}

/* Setters */
void GltfAnimationChannel::setTimings(std::vector<float> timinings) {
  mTimings = timinings;
}

void GltfAnimationChannel::setScalings(std::vector<glm::vec3> scalings) {
  mScaling = scalings;
}

void GltfAnimationChannel::setTranslations(std::vector<glm::vec3> tranlations) {
  mTranslations = tranlations;
}

void GltfAnimationChannel::setRotations(std::vector<glm::quat> rotations) {
  mRotations = rotations;
}

int GltfAnimationChannel::getTargetNode() {
  return mTargetNode;
}

ETargetPath GltfAnimationChannel::getTargetPath() {
  return mTargetPath;
}