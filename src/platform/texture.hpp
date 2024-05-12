#pragma once

#include <string>
#include <memory>

#include "platform/opengl.hpp"
#include <stb_image.hpp>

// A class to upload texture data
class Texture {
private:
  // Read texture data from a file and uploads it to the GPU
  // @param path The path of the texture file
  Texture(const std::string &path);

public:
  ~Texture();

  // Binds the texture to a texture unit
  void bind(unsigned int index);

  // Read texture data from a file and uploads it to the GPU
  // @param path The path of the texture file
  inline static auto Create(const std::string &path) {
    return std::shared_ptr<Texture>(new Texture(path));
  }

private:
  GLuint id;
};

// A class to handle textures that hold render data
class TextureRender {
private:
  /* Creates a texture for render data
   * @param resX The width of the texture
   * @param resY The height of the texture
   * @param format The format OpenGL uses for the texture
   */
  TextureRender(unsigned int resX, unsigned int resY, GLenum format);

public:
  ~TextureRender();

  // Binds the texture to a texture unit
  void bind(unsigned int index);
  // Returns the texture ID
  // @returns Returns the texture ID
  GLuint getID();

  /* Creates a texture for render data
   * @param resX The width of the texture
   * @param resY The height of the texture
   * @param format The format OpenGL uses for the texture
   */
  inline static auto Create(unsigned int resX, unsigned int resY, GLenum format) {
    return std::shared_ptr<TextureRender>(new TextureRender{resX, resY, format});
  }

private:
  GLuint id;
};
