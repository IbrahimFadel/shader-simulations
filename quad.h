#ifndef QUAD_H
#define QUAD_H

static const float quad_vertices[] = {
    -1,
    -1,
    0,
    1,
    -1,
    0,
    -1,
    1,
    0,
    -1,
    1,
    0,
    1,
    -1,
    0,
    1,
    1,
    0,
};

// static const float quad_vertices[] = {-1, -1, 0, };

// #include <stdint.h>

// static const float quad_vertices[] = {-1, -1, 0, // bottom left corner
//                                       -1, 1, 0,  // top left corner
//                                       1, 1, 0,   // top right corner
//                                       1, -1, 0}; // bottom right corner

// static const uint8_t quad_indices[] = {0, 1, 2,  // first triangle (bottom left - top left - top right)
//                                        0, 2, 3}; // second triangle (bottom left - top right - bottom right)

#endif