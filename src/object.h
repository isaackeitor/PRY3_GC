#pragma once
#include <glm/glm.hpp>
#include "intersect.h"
#include "material.h"

class Object
{
public:
  explicit Object(const Material &mat) : material(mat) {}
  virtual ~Object() = default;

  virtual Intersect rayIntersect(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const = 0;
  const Material &getMaterial() const { return material; }

protected:
  Material material;
};
