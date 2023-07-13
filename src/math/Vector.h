#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float x, y, z, w;
} Vec4;

extern void add_vec2(Vec2 *dest, Vec2 *v1, Vec2 *v2);

#endif