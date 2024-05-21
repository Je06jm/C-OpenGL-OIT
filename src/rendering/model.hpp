#pragma once

#include <array>
#include <string>
#include <vector>
#include <memory>

#include "platform/buffer.hpp"
#include "platform/opengl.hpp"
#include "platform/shader.hpp"
#include "platform/texture.hpp"
#include "rendering/transform.hpp"
#include "rendering/mesh.hpp"

class Model {
private:
  /* Creates a model from a file
   * @param path The model file path
   * @param base The base folder to use when looking for the material file
   */
  Model(const std::string &path, const std::string &base = "./");

public:
  // Draws the model using the given shader
  // @param shader The shader to use when drawing
  void draw(std::shared_ptr<Shader> shader);

  Transform transform;

  /* Creates a model from a file
   * @param path The model file path
   * @param base The base folder to use when looking for the material file
   */
  inline static auto create(const std::string &path, const std::string &base = "./") {
    return std::shared_ptr<Model>(new Model{path, base});
  }

private:
  // Each material get's it's own mesh
  std::vector<std::shared_ptr<Mesh>> meshes;

  struct Material {
    std::shared_ptr<Texture> diffused = nullptr;
    std::shared_ptr<Texture> specular = nullptr;
    std::shared_ptr<Texture> alpha = nullptr;

    union {
      struct {
        float r, g, b;
      };
      float values[3];
    } diffusedColor;

    union {
      struct {
        float r, g, b;
      };
      float values[3];
    } specularColor;

    float alphaValue;

    static constexpr unsigned int MASK_USE_DIFFUSED = (1 << 0);
    static constexpr unsigned int MASK_USE_SPECULAR = (1 << 1);
    static constexpr unsigned int MASK_USE_ALPHA = (1 << 2);

    unsigned int mask = 0;
  };

  // Holds material data
  std::vector<Material> materials;
};