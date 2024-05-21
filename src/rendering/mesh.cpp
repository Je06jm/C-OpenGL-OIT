#include "helper/log.hpp"
#include "rendering/mesh.hpp"

using namespace std;
using namespace glm;

#define USE_DIFFUSE_TEXTURE (1 << 0)
#define USE_SPECULAR_TEXTURE (1 << 1)
#define USE_ALPHA_TEXTURE (1 << 2)

#define POSITION_OFFSET 0
#define NORMAL_OFFSET 1
#define UV_OFFSET 2
#define COLOR_OFFSET 3

#define GET_POSITION(base) (base[POSITION_OFFSET])
#define GET_NORMAL(base) (base[NORMAL_OFFSET])
#define GET_UV(base) (base[UV_OFFSET])
#define GET_COLOR(base) (base[COLOR_OFFSET])

Mesh::Mesh(const vector<float> &positions, const vector<float> &normals,
  const vector<float> &uvs, const vector<float> &colors) :
  count(positions.size() / 3) {

  this->vao = BufferArray::create();

  // Upload model data
  this->vao->bind();

  // Position data
  this->buffers[0] =
      BufferData::create(sizeof(float) * positions.size(), positions.data());
  this->vao->setAttribute(0, 3, GL_FLOAT, 0, 0);

  // Normal data
  this->buffers[1] =
      BufferData::create(sizeof(float) * normals.size(), normals.data());
  this->vao->setAttribute(1, 3, GL_FLOAT, 0, 0);

  // UV data
  this->buffers[2] = BufferData::create(sizeof(float) * uvs.size(), uvs.data());
  this->vao->setAttribute(4, 2, GL_FLOAT, 0, 0);

  // Color data
  this->buffers[3] =
      BufferData::create(sizeof(float) * colors.size(), colors.data());
  this->vao->setAttribute(5, 3, GL_FLOAT, 0, 0);
}

void Mesh::draw() {
  // Draw
  this->vao->bind();

  glDrawArrays(GL_TRIANGLES, 0, this->count);

  this->vao->unbind();
}