#pragma once

#include "transform.hpp"

// Creates a camera object
class Camera {
public:
    /* Calculates the perspective matrix
     * @param FOV The field of view in degrees
     * @param aspect The aspect ratio calculated by (RESOLUTION_X/RESOLUTION_Y)
     * @param near The nearest distance from the camera that objects render
     * @param far The farthest distance from the camera that objects render
    */
    Camera(float FOV, float aspect, float near, float far);

    // Only the position and scale are used
    Transform transform;

    // Since this is a FPS camera, we will use
    // euler angles to simplify the math
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    // Returns the forward vector
    // @returns The forward vector
    glm::vec3 getForward();
    // Returns the right vector
    // @returns the right vector
    glm::vec3 getRight();
    // Returns the up vector
    // @returns the up vector
    glm::vec3 getUp();

    // Returns the perspective * view matrix
    // @returns The perspective * view matrix
    glm::mat4 getMatrix();
private:
    glm::mat4 P;
};
