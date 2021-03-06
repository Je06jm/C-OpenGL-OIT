#pragma once

#include "opengl.hpp"
#include "texture.hpp"

// Manages an OpenGL framebuffer
class Framebuffer {
public:
    /* Creates a framebuffer
     * @param resX The width of the framebuffer
     * @param resY The height of the framebuffer
    */
    Framebuffer(unsigned int resX, unsigned int resY);
    ~Framebuffer();

    // Attaches a TextureRender to use as a color buffer
    // @params texture A pointer to a TextureRender
    void attach(TextureRender* texture);

    // Builds and validates the framebuffer
    // @returns Returns true if the framebuffer is valid
    bool build();

    // Binds the framebuffer
    void bind();
    // Unbinds the framebuffer and binds the screen's framebuffer
    void unbind();
    // Clears the color and depth components
    void clear();

private:
    GLuint fbo;
    GLuint rbo;

    unsigned int colorAttachments = 0;
};
