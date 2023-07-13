#include "Vector.h"

void add_vec2(Vec2 *dest, Vec2 *v1, Vec2 *v2)
{
    dest->x = v1->x + v2->x;
    dest->y = v1->y + v2->y;
}