#pragma once

#include <glm/glm.hpp>

struct Intersect {
    glm::vec3 point;
    glm::vec3 normal;
    float distance;
    bool isIntersecting;

    Intersect() : point(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), distance(0.0f), isIntersecting(false) {}
    Intersect(bool intersect) : point(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), distance(0.0f), isIntersecting(intersect) {}
    Intersect(const glm::vec3& p, const glm::vec3& n, float d) : point(p), normal(n), distance(d), isIntersecting(true) {}
};