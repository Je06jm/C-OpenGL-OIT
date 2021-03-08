#pragma once

#include <array>
#include <string>
#include <vector>

#include "platform/buffer.hpp"
#include "platform/opengl.hpp"
#include "platform/shader.hpp"
#include "platform/texture.hpp"
#include "rendering/transform.hpp"

// Manages vertex data and uploads the transform
// data to a given shader
class Model {
public:
  /* Creates a model from a file
   * @param path The model file path
   * @param base The base folder to use when looking for the material file
   */
  Model(const std::string &path, const std::string &base = "./");
  ~Model();

  // Draws the model using the given shader
  // @param shader The shader to use when drawing
  void draw(Shader *shader);

  Transform transform;

private:
  BufferArray *vao;

  // Holds buffers. Position, Normal, UV, and Color
  std::array<BufferData *, 4> buffers;
  GLsizei count;

  /* Each material is held in an array stored in uniforms
   * Each texture/color/value in the material is stored
   * as an array, even when not used or does not exists.
   * Each texture is only stored once and can be assigned
   * to multiple sampler2Ds
   */

  // Hold material index
  BufferData *materialIndex;

  // Texture format: Diffuse, Specular, and Alpha
  std::vector<Texture *> textures;
  std::vector<unsigned int> mapsMask;

  // Material values
  std::vector<float> diffuseColors;
  std::vector<float> specularColors;
  std::vector<float> alphaValues;

  // Texture index
  std::vector<unsigned int> textureIndexes;
};
