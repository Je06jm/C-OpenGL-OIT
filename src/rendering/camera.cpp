#include "rendering/camera.hpp"

using namespace glm;

Camera::Camera(float FOV, float aspect, float near, float far) {
  this->P = perspective(radians(FOV), aspect, near, far);
}

vec3 Camera::getForward() {
  // Calculates the forward vector
  vec3 forward =
      vec3(cos(this->rotation.x) * sin(this->rotation.y), sin(this->rotation.x),
           cos(this->rotation.x) * cos(this->rotation.y));

  return forward;
}

vec3 Camera::getRight() {
  // Calculates the right vector
  vec3 right = vec3(sin(this->rotation.y - pi<float>() / 2.0f), 0.0f,
                    cos(this->rotation.y - pi<float>() / 2.0f));

  return right;
}

vec3 Camera::getUp() {
  // Calculates the up vector
  vec3 forward = this->getForward();
  vec3 right = this->getRight();
  vec3 up = cross(right, forward);

  return -up;
}

mat4 Camera::getMatrix() {
  // Use glm::lookAt to calculate the view matrix
  vec3 forward = this->getForward();
  vec3 right = this->getRight();
  vec3 up = this->getUp();

  mat4 V =
      lookAt(this->transform.position, this->transform.position + forward, up);
  return this->P * V;
}
