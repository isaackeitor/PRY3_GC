#include "camera.h"

// Constructor for the Camera class.
// Initializes the position, target and rotationSpeed based on the input parameters.
Camera::Camera(glm::vec3 pos, glm::vec3 tar, float rotSpeed)
    : position(pos), target(tar), rotationSpeed(rotSpeed) {}

// The rotate function takes in a change in x (deltaX) and a change in y (deltaY) 
// and adjusts the orientation of the camera based on this input.
// It uses quaternions to perform the rotation. Quaternions are a way to perform 3D rotations without suffering from gimbal lock.
void Camera::rotate(float deltaX, float deltaY) {
    // Create quaternions representing the rotation around the y and x axis
    glm::quat quatAroundY = glm::angleAxis(glm::radians(deltaX * rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat quatAroundX = glm::angleAxis(glm::radians(deltaY * rotationSpeed), glm::vec3(1.0f, 0.0f, 0.0f));

    // Rotate the position of the camera around the target.
    position = target + quatAroundY * (position - target);  // First rotate around the Y-axis
    position = target + quatAroundX * (position - target);  // Then rotate around the X-axis
}

// This function adjusts the position of the camera along the z-axis based on the input deltaZ.
void Camera::move(float deltaZ) {
    glm::vec3 dir = glm::normalize(target - position);  // Get the direction vector
    position += dir * deltaZ;  // Move the camera
}