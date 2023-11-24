#pragma once
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    Color(Uint8 red = 0, Uint8 green = 0, Uint8 blue = 0, Uint8 alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    // Overload the + operator to add colors
    Color operator+(const Color& other) const {
        return Color(
            std::min(255, int(r) + int(other.r)),
            std::min(255, int(g) + int(other.g)),
            std::min(255, int(b) + int(other.b)),
            std::min(255, int(a) + int(other.a))
        );
    }

    // Overload the * operator to scale colors by a factor
    Color operator*(float factor) const {
        return Color(
            std::clamp(static_cast<Uint8>(r * factor), Uint8(0), Uint8(255)),
            std::clamp(static_cast<Uint8>(g * factor), Uint8(0), Uint8(255)),
            std::clamp(static_cast<Uint8>(b * factor), Uint8(0), Uint8(255)),
            std::clamp(static_cast<Uint8>(a * factor), Uint8(0), Uint8(255))
        );
    }

    // Declare the << operator to print colors
    friend std::ostream& operator<<(std::ostream& os, const Color& color);

    // Friend function to allow float * Color
    friend Color operator*(float factor, const Color& color);
};