#include "skybox.h"
#include <SDL_image.h>

Skybox::Skybox(const std::string& textureFile) {
    loadTexture(textureFile);
}

Skybox::~Skybox() {
    SDL_FreeSurface(texture);
}

void Skybox::loadTexture(const std::string& textureFile) {
    /*
    texture = IMG_Load(textureFile.c_str());
    if (!texture) {
        throw std::runtime_error("Failed to load skybox texture: " + std::string(IMG_GetError()));
    }
    */
    
    SDL_Surface* rawTexture = IMG_Load(textureFile.c_str());
    if (!rawTexture) {
        throw std::runtime_error("Failed to load skybox texture: " + std::string(IMG_GetError()));
    }
    // Convert the loaded image to RGB format
    texture = SDL_ConvertSurfaceFormat(rawTexture, SDL_PIXELFORMAT_RGB24, 0);
    if (!texture) {
        SDL_FreeSurface(rawTexture);
        throw std::runtime_error("Failed to convert skybox texture to RGB: " + std::string(SDL_GetError()));
    }
    SDL_FreeSurface(rawTexture);
}

Color Skybox::getColor(const glm::vec3& direction) const {
    // Convert direction vector to spherical coordinates
    float phi = atan2(direction.z, direction.x);
    float theta = acos(direction.y);
    
    // Convert spherical coordinates to texture coordinates
    float u = 0.5f + phi / (2 * M_PI);
    float v = theta / M_PI;
    
    // Map texture coordinates to pixel coordinates
    int x = static_cast<int>(u * texture->w) % texture->w;
    int y = static_cast<int>(v * texture->h) % texture->h;
    
    // Ensure x and y are within the valid range
    x = std::max(0, std::min(texture->w - 1, x));
    y = std::max(0, std::min(texture->h - 1, y));
    
    // Get pixel color from texture
    Uint8 r, g, b;
    Uint8* pixel = &((Uint8*)texture->pixels)[3 * (y * texture->w + x)];
    r = pixel[0];
    g = pixel[1];
    b = pixel[2];
    
    return Color(r, g, b);
}
