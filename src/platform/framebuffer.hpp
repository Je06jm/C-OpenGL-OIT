#pragma once

#include <memory>

#include "platform/opengl.hpp"
#include "platform/texture.hpp"

// Manages an OpenGL framebuffer
class Framebuffer {
private:
  /* Creates a framebuffer
   * @param resX The width of the framebuffer
   * @param resY The height of the framebuffer
   */
  Framebuffer(unsigned int resX, unsigned int resY);

public:
  ~Framebuffer();

  // Attaches a TextureRender to use as a color buffer
  // @params texture A pointer to a TextureRender
  void attach(std::shared_ptr<TextureRender> texture);

  // Builds and validates the framebuffer
  // @returns Returns true if the framebuffer is valid
  bool build();

  // Binds the framebuffer
  void bind();
  // Unbinds the framebuffer and binds the screen's framebuffer
  void unbind();
  // Clears the color and depth components
  void clear();

  /* Creates a framebuffer
   * @param resX The width of the framebuffer
   * @param resY The height of the framebuffer
   */
  inline static auto Create(unsigned int resX, unsigned int resY) {
    return std::shared_ptr<Framebuffer>(new Framebuffer{resX, resY});
  }

private:
  GLuint fbo;
  GLuint rbo;

  unsigned int colorAttachments = 0;
};
