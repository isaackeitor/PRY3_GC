#include "cube.h"

Cube::Cube(const glm::vec3 &min, const glm::vec3 &max, const Material &mat)
    : min(min), max(max), Object(mat) {}

glm::vec3 Cube::normal(const glm::vec3 &intersectPoint) const {
    // Un pequeño valor de sesgo para manejar problemas de precisión numérica
    const float bias = 1e-4;

    // Comprobar cada cara del cubo para determinar a cuál cara está más cerca el punto de intersección
    // Se establecerá la normal en función de la cara más cercana
    if (std::fabs(intersectPoint.x - min.x) < bias) {
        // El punto de intersección está más cerca de la cara izquierda del cubo
        return glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    if (std::fabs(intersectPoint.x - max.x) < bias) {
        // El punto de intersección está más cerca de la cara derecha del cubo
        return glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (std::fabs(intersectPoint.y - min.y) < bias) {
        // El punto de intersección está más cerca de la cara inferior del cubo
        return glm::vec3(0.0f, -1.0f, 0.0f);
    }
    if (std::fabs(intersectPoint.y - max.y) < bias) {
        // El punto de intersección está más cerca de la cara superior del cubo
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (std::fabs(intersectPoint.z - min.z) < bias) {
        // El punto de intersección está más cerca de la cara frontal del cubo
        return glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (std::fabs(intersectPoint.z - max.z) < bias) {
        // El punto de intersección está más cerca de la cara trasera del cubo
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    // En caso de que no se ajuste a ninguna cara (situación poco probable), devolver un vector normal por defecto
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

Intersect Cube::rayIntersect(const glm::vec3 &origenRayo, const glm::vec3 &direccionRayo) const {
    // Calcular los inversos de la dirección del rayo para evitar divisiones repetidas
    glm::vec3 invDir = glm::vec3(1.0f) / direccionRayo;

    // Calcular distancias a las caras del cubo en cada eje
    float tMinX = (min.x - origenRayo.x) * invDir.x;
    float tMaxX = (max.x - origenRayo.x) * invDir.x;
    float tMinY = (min.y - origenRayo.y) * invDir.y;
    float tMaxY = (max.y - origenRayo.y) * invDir.y;
    float tMinZ = (min.z - origenRayo.z) * invDir.z;
    float tMaxZ = (max.z - origenRayo.z) * invDir.z;

    // Asegurarse de que tMin sea siempre menor que tMax en cada eje
    float tMin = std::max(std::min(tMinX, tMaxX), std::max(std::min(tMinY, tMaxY), std::min(tMinZ, tMaxZ)));
    float tMax = std::min(std::max(tMinX, tMaxX), std::min(std::max(tMinY, tMaxY), std::max(tMinZ, tMaxZ)));

    // Si los intervalos no se solapan, no hay intersección
    if (tMin > tMax || tMax < 0) {
        return Intersect{false}; // No hay intersección
    }

    // Calcular el punto de intersección y la normal en ese punto
    glm::vec3 puntoInterseccion = origenRayo + tMin * direccionRayo;

    return Intersect{puntoInterseccion, normal(puntoInterseccion), tMin};
}
