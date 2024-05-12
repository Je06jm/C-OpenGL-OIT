#include "rendering/light.hpp"

#include <string>

using namespace std;

void Light::set(std::shared_ptr<Shader> shader, int index) {
  // Calculate light name in the shader
  string name = string("lights[") + to_string(index) + "]";

  shader->setUniformVec3(name + ".position", this->position);
  shader->setUniformVec3(name + ".color", this->color);
  shader->setUniformFloat(name + ".strength", strength);
}
