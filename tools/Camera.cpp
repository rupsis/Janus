#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewMatrix(OGLRenderData &renderData) {
  float azimRad = glm::radians(renderData.rdViewAzimuth);
  float elevRad = glm::radians(renderData.rdViewElevation);

  float sinAzim = glm::sin(azimRad);
  float cosAzim = glm::cos(azimRad);
  float sinElev = glm::sin(elevRad);
  float cosElev = glm::cos(elevRad);

  mViewDirection = glm::normalize(glm::vec3(
      // Left - right
      sinAzim * cosElev,
      // Up / Down
      sinElev,
      // into the screen, -z is towards the screen
      -cosAzim * cosElev));

  mRightDirection = glm::normalize(glm::cross(mViewDirection, mWorldUpVector));
  mUpDirection = glm::normalize(glm::normalize(glm::cross(mRightDirection, mViewDirection)));

  renderData.rdCameraWorldPosition +=
      (renderData.rdMoveForward * renderData.rdTickDiff * mViewDirection) +
      (renderData.rdMoveRight * renderData.rdTickDiff * mRightDirection) +
      (renderData.rdMoveUp * renderData.rdTickDiff * mUpDirection);

  // Return 4x4 lookat matrix
  return glm::lookAt(renderData.rdCameraWorldPosition,
                     renderData.rdCameraWorldPosition + mViewDirection,
                     mWorldUpVector);
}