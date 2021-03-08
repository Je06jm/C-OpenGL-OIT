#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// A class that handles transforms
class Transform {
public:
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

  /* Rotates the transform along a given axis
   * @param axis The axis to rotate along
   * @param angle The number of degrees to rotate
   */
  void rotateAxis(glm::vec3 axis, float angle);
  // Rotates the transform by a euler rotation
  // @param rotation The euler rotation to rotate by
  void rotateEuler(glm::vec3 rotation);
  // Sets the current rotation
  // @param rotation The euler rotation to set rotation to
  void setRotation(glm::vec3 rotation);

  /* Returns the matrix for OpenGL
   * @param isCamera If true, the matrix is rotation * translation * scale. If
   * false, the matrix is translation * rotation * scale
   * @returns The OpenGL matrix
   */
  glm::mat4 getMatrix(bool isCamera = false);
};
