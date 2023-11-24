#pragma once
#include "color.h"
#include <algorithm>

struct Material
{
  Color diffuse;
  float albedo;
  float specularAlbedo;
  float specularCoefficient; // The specular coefficient
  float reflectivity;        // The reflectivity of the material
  float transparency;        // The transparency of the material
  float refractionIndex;

  Material(const Color &color, float albedo, float specularAlbedo, float specCoef, float reflectivity = 0, float transparency = 0, float refractionIndex = 0)
      : diffuse(color),
        albedo(albedo),
        specularAlbedo(specularAlbedo),
        specularCoefficient(specCoef),
        reflectivity(reflectivity),
        transparency(transparency),
        refractionIndex(refractionIndex)
  {
  }
};