#include <stb_image.h>

#include "Texture.h"

bool Texture::loadTexture(std::string textureFilename, bool flipImage) {
  int mTexWidth, mTexHeight, mNumberOfChannels;
  stbi_set_flip_vertically_on_load(flipImage);
  unsigned char *textureData = stbi_load(
      textureFilename.c_str(), &mTexWidth, &mTexHeight, &mNumberOfChannels, 0);

  if (!textureData) {
    // If image doesn't load, free the memory else we create a memory leak.
    stbi_image_free(textureData);
    return false;
  }

  glGenTextures(1, &mTexture);
  glBindTexture(GL_TEXTURE_2D, mTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  //  Load from system memory to GPU
  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA, mTexWidth, mTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
  glGenerateMipmap(GL_TEXTURE_2D);

  // unbind to avoid accidental changes.
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(textureData);
  return true;
}

void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture::unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::cleanup() {
  glDeleteTextures(1, &mTexture);
}