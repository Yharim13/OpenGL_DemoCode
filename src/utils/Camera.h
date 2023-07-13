#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct {
    mat4 m_proj;

    mat4 m_view;

    vec3 m_pos;

    float m_pitch, m_yaw;

    vec3 m_dir;

    vec3 m_up, m_right, m_forward, m_front;
    
    vec3 m_side_translation;
} Camera;

extern void init_camera(Camera* self);

extern void update_camera(Camera* self, vec2 mouse_delta, vec3 translation);

extern void init_projection(Camera* self, float aspect_ratio, float fov, float z_near, float z_far);

#endif