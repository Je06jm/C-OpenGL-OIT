#pragma once

#include <string>

#include "opengl.hpp"
#include "stb_image.hpp"

// A class to upload texture data
class Texture {
public:
    // Read texture data from a file and uploads it to the GPU
    // @param path The path of the texture file
    Texture(const std::string& path);

    ~Texture();

    // Binds the texture to a texture unit
    void bind(unsigned int index);
private:
    GLuint id;
};

// A class to handle textures that hold render data
class TextureRender {
public:
    /* Creates a texture for render data
     * @param resX The width of the texture
     * @param resY The height of the texture
     * @param format The format OpenGL uses for the texture
    */
    TextureRender(unsigned int resX, unsigned int resY, GLenum format);
    ~TextureRender();

    // Binds the texture to a texture unit
    void bind(unsigned int index);
    // Returns the texture ID
    // @returns Returns the texture ID
    GLuint getID();
private:
    GLuint id;
};
