#pragma once

#include <array>
#include <vector>
#include <memory>

#include "platform/buffer.hpp"
#include "platform/opengl.hpp"
#include "platform/shader.hpp"
#include "platform/texture.hpp"

// Manages vertex data
class Mesh {
private:
  Mesh(const std::vector<float> &positions, const std::vector<float> &normals,
    const std::vector<float> &uvs, const std::vector<float> &colors);

public:
  void draw();

  inline static auto create(const std::vector<float> &positions, const std::vector<float> &normals,
    const std::vector<float> &uvs, const std::vector<float> &colors) {
    return std::shared_ptr<Mesh>(new Mesh{positions, normals, uvs, colors});
  }

private:
  std::shared_ptr<BufferArray> vao;

  // Holds buffers. Position, Normal, UV, and Color
  std::array<std::shared_ptr<BufferData>, 4> buffers;
  const GLsizei count;
};