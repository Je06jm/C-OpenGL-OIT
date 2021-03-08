#pragma once

#include "platform/opengl.hpp"

// Manages an OpenGL vertex array object
class BufferArray {
public:
  // Creates a vertex array object
  BufferArray();
  // Destroyes the vertex array object
  ~BufferArray();

  // Bind the vertex array object
  void bind();
  // Unbinds the vertex array object
  void unbind();

  /* Allows OpenGL vertex attributes to be defined
   * @param attribute The attribute number
   * @param components The number of components each item has. (Vec3 has 3
   * components)
   * @param type The data type (FLOAT, DOUBLE, etc.)
   * @param stride The stride between items. A stride of zero will use the
   * default stride used by OpenGL
   * @param offset The offset from the start of the attribute data
   */
  void setAttribute(unsigned int attribute, unsigned int components,
                    GLenum type, size_t stride, size_t offset);

  /* Allows OpenGL vertex integer attributes to be defined
   * @param attribute The attribute number
   * @param components The number of components each item has. (Vec3 has 3
   * components)
   * @param type The data type (INT, LONG, etc.)
   * @param stride The stride between items. A stride of zero will use the
   * default stride used by OpenGL
   * @param offset The offset from the start of the attribute data
   */
  void setAttributeI(unsigned int attribute, unsigned int components,
                     GLenum type, size_t stride, size_t offset);

  // Returns the maximum number of attributes allowed
  // @returns The maximum number of attributes allowed
  static unsigned int getMaxAttributes();

private:
  GLuint vao;

  // Each bit represents a seperate attribute. For example,
  // bit 0 is attribute 0 and bit 1 is attribute 1
  unsigned long attributes = 0;
};

// Buffer is not ment to be used directly. It only handles the
// creating and destruction of a generic OpenGL buffer object
class Buffer {
public:
  // Creates a generic buffer object
  Buffer();
  // Destroyes the buffer object
  ~Buffer();

  // Binds the buffer object
  virtual void bind();
  // Unbinds the buffer object
  virtual void unbind();

protected:
  GLuint buffer;
};

// Vertex data is uploaded and managed using this class
class BufferData : public Buffer {
public:
  /* Uploads data onto the GPU
   * @param size The length of the data
   * @param data The pointer to the data
   * @param usage OpenGL usage hint (GL_STATIC_DRAW)
   */
  BufferData(size_t size, void *data, GLenum usage = GL_STATIC_DRAW);

  // Binds the buffer object
  void bind() override;
  // Unbinds the buffer object
  void unbind() override;
};

// This class manages OpenGL shader storage buffer objects.
class BufferStorage : public Buffer {
public:
  /* Checks for SSBO support. Does not define a initial size.
   * Use resize to create a buffer
   * @param usage OpenGL usage hint (GL_DYNAMIC_COPY)
   */
  BufferStorage(GLenum usage = GL_DYNAMIC_COPY);

  // Binds the buffer object
  void bind() override;
  // Unbinds the buffer object
  void unbind() override;

  /* SSBOs are bound to a global location and do not need
   * to be set per shader
   * @param location Shader binding location
   */
  void setLocation(size_t location);

  // Sets the entire buffer to zero
  void clear();

  /* Tells OpenGL to reallocate the buffer with a new size.
   * Data is not copied between the old and new buffer memory
   * @param newSize The new size of the buffer
   */
  void resize(size_t newSize);

  // Sets a memory barrier for SSBOs
  void barrier();

private:
  GLenum usage;
};

// An OpenGL atomic counter is managed with this class
class BufferCounter : public Buffer {
public:
  /* Checks fot atomic counter support. Creates an atomic counter
   * @param resetValue The value to reset the counter to
   * @param usage OpenGL usage hint (GL_MAP_WRITE_BIT | GL_MAP_READ_BIT |
   * GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
   */
  BufferCounter(unsigned int resetValue = 0,
                GLbitfield usage = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT |
                                   GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

  ~BufferCounter();

  // Bind the buffer object
  void bind() override;
  // Unbinds the buffer object
  void unbind() override;

  /* Atomic Counters are bound to a global location and do
   * not need to be set per shader
   * @param location Shader binding location
   */
  void setLocation(size_t location);

  // Returns the current value of the atomic counter
  // @returns The current value of the atomic counter
  unsigned int read();

  // Sets the value of the atomic counter to the resetValue
  void reset();

private:
  // This makes the client wait for the fence. This is called
  // before the ptr is used
  void fence();
  unsigned int resetValue;

  // This is a persistent pointer from OpenGL
  unsigned int *ptr;
};
