#include "color.h"

// Definition of friend functions outside of the class

std::ostream& operator<<(std::ostream& os, const Color& color) {
    os << "Color(" << static_cast<int>(color.r) << ", " 
       << static_cast<int>(color.g) << ", " 
       << static_cast<int>(color.b) << ", " 
       << static_cast<int>(color.a) << ")";
    return os;
}

Color operator*(float factor, const Color& color) {
    return color * factor;
}