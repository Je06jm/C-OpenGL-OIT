#include "platform/buffer.hpp"

#include "helper/log.hpp"

// The max number of attributes our application can support
#define MAX_ATTRIBUTES (sizeof(unsigned long) * 8)

BufferArray::BufferArray() {
    glGenVertexArrays(1, &this->vao);
}

BufferArray::~BufferArray() {
    glDeleteVertexArrays(1, &this->vao);
}

void BufferArray::bind() {
    glBindVertexArray(this->vao);

    // Check bits to see required attributes and enable them
    for (unsigned long i = 0; i < MAX_ATTRIBUTES; i++) {
        if (this->attributes & (1 << i)) {
            glEnableVertexAttribArray(i);
        }
    }
}

void BufferArray::unbind() {
    // Disable attributes based on bits in attributes
    for (unsigned long i = 0; i < MAX_ATTRIBUTES; i++) {
        if (this->attributes & (1 << i)) {
            glDisableVertexAttribArray(i);
        }
    }

    glBindVertexArray(0);
}

void BufferArray::setAttribute(unsigned int attribute, unsigned int components, GLenum type, size_t stride, size_t offset) {
    // Records the attribute and tell OpenGL about it
    this->attributes |= (1 << attribute);

    glVertexAttribPointer(attribute, components, type, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(attribute);
}

void BufferArray::setAttributeI(unsigned int attribute, unsigned int components, GLenum type, size_t stride, size_t offset) {
    // Records the integer attribute and tell OpenGL about it
    this->attributes |= (1 << attribute);

    glVertexAttribIPointer(attribute, components, type, stride, (void*)offset);
    glEnableVertexAttribArray(attribute);
}

unsigned int BufferArray::getMaxAttributes() {
    // Because of the number of bits in attributes, the software
    // can only support a maximum of MAX_ATTRIBUTES. OpenGL may
    // report a fewer number of supported attributes. We want
    // to return the fewer of the two

    int attributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attributes);

    if (attributes >= MAX_ATTRIBUTES) {
        attributes = MAX_ATTRIBUTES - 1;
    }

    return (unsigned int)attributes;
}


Buffer::Buffer() {
    glGenBuffers(1, &this->buffer);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &this->buffer);
}

void Buffer::bind() {}
void Buffer::unbind() {}


BufferData::BufferData(size_t size, void* data, GLenum usage) {
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void BufferData::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
}

void BufferData::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


BufferStorage::BufferStorage(GLenum usage) {
    // Check for SSBO support and exit if they are not
    if (!GLAD_GL_ARB_shader_storage_buffer_object) {
        critical("Cannot find extension GL_ARB_shader_storage_buffer_object\n");
    }
    this->usage = usage;
}

void BufferStorage::bind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->buffer);
}

void BufferStorage::unbind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void BufferStorage::setLocation(size_t location) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, this->buffer);
}

void BufferStorage::clear() {
    glClearNamedBufferData(this->buffer, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
}

void BufferStorage::resize(size_t newSize) {
    this->bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, newSize, nullptr, this->usage);
}

void BufferStorage::barrier() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


BufferCounter::BufferCounter(unsigned int resetValue, GLbitfield usage) {
    // Checks for atomic counter support and exit if they are not
    if (!GLAD_GL_ARB_shader_atomic_counters) {
        critical("Cannot find extension GL_ARB_shader_atomic_counters\n");
    }

    // Creates a counter
    this->bind();
    this->resetValue = resetValue;
    glBufferStorage(GL_ATOMIC_COUNTER_BUFFER, sizeof(int), &resetValue, usage);

    // Maps the counter to the clients memory
    this->ptr = (unsigned int*)glMapBufferRange(
        GL_ATOMIC_COUNTER_BUFFER,
        0,
        4,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
    );
}

BufferCounter::~BufferCounter() {
    this->bind();

    // Frees the pointer
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
}

void BufferCounter::bind() {
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, this->buffer);
}

void BufferCounter::unbind() {
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void BufferCounter::setLocation(size_t location) {
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, location, this->buffer);
}

unsigned int BufferCounter::read() {
    this->fence();
    return *this->ptr;
}

void BufferCounter::reset() {
    this->fence();
    *this->ptr = this->resetValue;
}

void BufferCounter::fence() {
    // Creates a fence object, and hang until OpenGL processes it
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, GL_ZERO);
    glClientWaitSync(sync, GL_ZERO, -1);
}
