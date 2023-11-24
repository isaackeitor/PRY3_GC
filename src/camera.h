#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 target;
    float rotationSpeed;

    Camera(glm::vec3 pos, glm::vec3 tar, float rotSpeed);
    void rotate(float deltaX, float deltaY);
    void move(float deltaZ);
};