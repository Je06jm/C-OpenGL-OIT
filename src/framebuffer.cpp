#include "framebuffer.hpp"

Framebuffer::Framebuffer(unsigned int resX, unsigned int resY) {
    glGenFramebuffers(1, &this->fbo);
    this->bind();

    // Sets a renderbuffer as the depth and stencil buffer
    glGenRenderbuffers(1, &this->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resX, resY);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

    this->unbind();
}

Framebuffer::~Framebuffer() {
    glDeleteRenderbuffers(1, &this->rbo);
    glDeleteFramebuffers(1, &this->fbo);
}

void Framebuffer::attach(TextureRender* texture) {
    this->bind();

    glBindTexture(GL_TEXTURE_2D, texture->getID());
    
    // Sets texture filtering to something more useful
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attaches the texture to the framebuffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, 
        GL_COLOR_ATTACHMENT0 + this->colorAttachments,
        GL_TEXTURE_2D,
        texture->getID(),
        0
    );

    this->colorAttachments++;
}

bool Framebuffer::build() {
    this->bind();

    // Checks to see if the framebuffer is valid
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        // Enable the color buffers that are used in the framebuffer
        GLenum* attachments = new GLenum[this->colorAttachments];

        for (size_t i = 0; i < this->colorAttachments; i++) {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        glDrawBuffers(this->colorAttachments, attachments);

        delete attachments;

        return true;
    }

    return false;
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
