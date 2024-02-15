#include "Framebuffer.h"
#include "Logger.h"

bool Framebuffer::init(unsigned int width, unsigned int height) {
  mBufferWidth = width;
  mBufferHeight = height;

  // '&' means we're writing to the member variable
  glGenFramebuffers(1, &mBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);

  glGenTextures(1, &mColorTex);
  glBindTexture(GL_TEXTURE_2D, mColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  // Additional properties since some drivers need them
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Unbind the texture (set invalid ID) to avoid further modifications
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mColorTex, 0);

  // depth buffer
  glGenRenderbuffers(1, &mDepthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

  // setup is finished, unbind render/frame buffer
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Need to ensure all components are in place before draw(), else an error will occur.
  return checkComplete();
}

bool Framebuffer::checkComplete() {
  glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);
  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    Logger::log(1, "%s: OpenGL framebuffer issue (%i)\n", __FUNCTION__, result);
    return false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}

bool Framebuffer::resize(unsigned int newWidth, unsigned int newHeight) {
  mBufferWidth = newWidth;
  mBufferHeight = newHeight;

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDeleteTextures(1, &mColorTex);
  glDeleteRenderbuffers(1, &mDepthBuffer);
  glDeleteFramebuffers(1, &mBuffer);

  return init(newWidth, newHeight);
}

void Framebuffer::bind() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBuffer);
}

void Framebuffer::unbind() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Framebuffer::cleanup() {
  unbind();
  glDeleteTextures(1, &mColorTex);
  glDeleteRenderbuffers(1, &mDepthBuffer);
  glDeleteFramebuffers(1, &mBuffer);
}

void Framebuffer::drawToScreen() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, mBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // Blitting is a memory copy
  glBlitFramebuffer(0,
                    0,
                    mBufferWidth,
                    mBufferHeight,
                    0,
                    0,
                    mBufferWidth,
                    mBufferHeight,
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
