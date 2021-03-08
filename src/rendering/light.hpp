#pragma once

#include "platform/shader.hpp"
#include <glm/glm.hpp>

// A class to hold light data. Also
// helps upload lighting data to shaders
class Light {
public:
  // Light position
  glm::vec3 position;
  // Light color
  glm::vec3 color;
  // Light strength
  float strength;

  /* Uploads lighting data to the given shader
   * @param shader The shader to upload the light data to
   * @param index The light index within the shader to upload to
   */
  void set(Shader *shader, int index);
};