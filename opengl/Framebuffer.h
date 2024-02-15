#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

/* The final picture on the screen is created by a framebuffer,
 * the intermediate results of rendering steps are stored in the framebuffer as well.
 */

class Framebuffer {
 public:
  bool init(unsigned int width, unsigned int height);
  bool resize(unsigned int width, unsigned int height);

  /*
   * Bind() enables the drawing to the framebuffer, while unbind() disables it.
   * Even when using a single buffer (multiple buffers is "deferred rendering"),
   * it's best practice to bind/unbind to avoid surprise with draw/clean calls.
   */
  void bind();
  void unbind();

  void drawToScreen();
  void cleanup();

 private:
  unsigned int mBufferWidth = 640;
  unsigned int mBufferHeight = 480;
  GLuint mBuffer = 0;
  GLuint mColorTex = 0;
  GLuint mDepthBuffer = 0;
  bool checkComplete();
};