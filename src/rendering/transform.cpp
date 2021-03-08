#include "rendering/transform.hpp"
#include <glm/gtx/quaternion.hpp>

using namespace glm;

void Transform::rotateAxis(vec3 axis, float angle) {
  quat rot = angleAxis(glm::radians(angle), axis);
  this->rotation *= rot;
}

void Transform::rotateEuler(vec3 rotation) {
  // Preforms three rotations along each axis, using each component
  // as the angle
  this->rotateAxis(vec3(1.0f, 0.0f, 0.0f), rotation.x);
  this->rotateAxis(vec3(0.0f, 1.0f, 0.0f), rotation.y);
  this->rotateAxis(vec3(0.0f, 0.0f, 1.0f), rotation.z);
}

void Transform::setRotation(vec3 rotation) {
  this->rotation = quat(0.0f, 0.0f, 0.0f, 1.0f);
  this->rotateEuler(rotation);
}

mat4 Transform::getMatrix(bool isCamera) {
  // Calculate matrixes
  mat4 trans = translate(mat4(1.0f), this->position);
  mat4 rot = toMat4(this->rotation);
  mat4 scale = glm::scale(mat4(1.0f), this->scale);

  // Calculate the final matrix
  if (isCamera) {
    return rot * trans * scale;
  } else {
    return trans * rot * scale;
  }
}
