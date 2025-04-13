#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

using color = vec3;

inline void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

	// clamp each color component to the range [0, 1]
    r = std::min(r, 1.0);
    g = std::min(g, 1.0);
    b = std::min(b, 1.0);

    // translate the [0,1] component values to the byte range [0,255]
    int red_byte = int(255.999 * r);
    int green_byte = int(255.999 * g);
    int blue_byte = int(255.999 * b);

    // write out the pixel color components
    out << red_byte << ' ' << green_byte << ' ' << blue_byte << '\n';
}

#endif
