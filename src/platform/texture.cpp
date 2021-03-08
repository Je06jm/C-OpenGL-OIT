#include "platform/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

#include "helper/log.hpp"

using namespace std;

Texture::Texture(const string& path) {
    // Creates a texture object
    info("Loading texture: %s\n", path.c_str());
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    // Load data
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    
    if (data == nullptr) {
        critical("Unknown path: %s\n", path.c_str());
    }

    // Uploads the texture data
    switch (nrChannels) {
        case 1: // BW image
            {
                GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        break;
        case 2: // RG image
            {
                GLint swizzleMask[] = {GL_RED, GL_GREEN, GL_GREEN, GL_GREEN};
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
        break;
        case 3: // RGB image
            {
                GLint swizzleMask[] = {GL_RED, GL_GREEN, GL_BLUE, GL_BLUE};
                glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        break;
        case 4: // RGBA image
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        break;
        default: // Unknown image
            critical("Unknown image channel for %s: %i\n", path.c_str(), nrChannels);
        break;
    }

    stbi_image_free(data);

    // Set the texture's parameters
    glGenerateMipmap(GL_TEXTURE_2D);

    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture() {
    glDeleteTextures(1, &this->id);
}

void Texture::bind(unsigned int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, this->id);
}

TextureRender::TextureRender(unsigned int resX, unsigned int resY, GLenum format) {
    // Create a texture object
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    
    if (format == GL_DONT_CARE) {
        format = GL_RGBA8;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, resX, resY, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

TextureRender::~TextureRender() {
    glDeleteTextures(1, &this->id);
}

void TextureRender::bind(unsigned int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, this->id);
}

GLuint TextureRender::getID() {
    return this->id;
}
