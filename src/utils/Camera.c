#include "Camera.h"
#include "Constants.h"

static inline void _calculate_right(Camera *self)
{
    glm_cross(self->m_dir, self->m_up, self->m_right);
    glm_normalize(self->m_right);
}

void init_camera(Camera *self)
{
    self->m_up[DIR_X] = 0.0f; self->m_up[DIR_Y] = 1.0f; self->m_up[DIR_Z] = 0.0f;

    self->m_dir[DIR_X] = 1.0f; self->m_dir[DIR_Y] = 0.0f; self->m_dir[DIR_Z] = 0.0f;

    self->m_pos[DIR_X] = 0.0f; self->m_pos[DIR_Y] = 0.0f; self->m_pos[DIR_Z] = 0.0f;

    float init_fov = glm_rad(45.0f);
    float init_znear = 0.1f, init_zfar = 100.0f;

    glm_perspective(init_fov, 1.0f, init_znear, init_zfar, self->m_proj);

    _calculate_right(self);
}

void init_projection(Camera* self, float aspect_ratio, float fov, float  z_near, float z_far)
{
    glm_perspective(glm_rad(fov), aspect_ratio, z_near, z_far, self->m_proj);
}

void update_camera(Camera* self, vec2 mouse_delta, vec3 relative_translation)
{
    //=======TRANSLATE=======

    //===FORWARD TRANSLATION===
    self->m_forward[DIR_X] = self->m_dir[DIR_X]; self->m_forward[DIR_Y] = 0.0f; self->m_forward[DIR_Z] = self->m_dir[DIR_Z]; glm_normalize(self->m_forward);
    glm_vec3_scale(self->m_forward, relative_translation[DIR_Z], self->m_forward);

    glm_vec3_add(self->m_pos, self->m_forward, self->m_pos);

    //===VERTICAL TRANSLATION===
    self->m_pos[DIR_Y] += relative_translation[DIR_Y];

    //===SIDEWAYS TRANSLATION===
    glm_vec3_scale(self->m_right, relative_translation[DIR_X], self->m_side_translation);

    glm_vec3_add(self->m_pos, self->m_side_translation, self->m_pos);

    //=======ROTATE======

    float mouse_sensitivity = 0.5f;

    self->m_yaw += mouse_delta[DIR_X] * mouse_sensitivity;
    self->m_pitch -= mouse_delta[DIR_Y] * mouse_sensitivity;

    if (self->m_pitch > 89.0f)
    {
        self->m_pitch = 89.0f;
    }
    else if (self->m_pitch < -89.0f)
    {
        self->m_pitch = -89.0f;
    }

    self->m_dir[0] = cosf(glm_rad(self->m_yaw)) * cosf(glm_rad(self->m_pitch));
	self->m_dir[1] = sinf(glm_rad(self->m_pitch));
	self->m_dir[2] = sinf(glm_rad(self->m_yaw)) * cosf(glm_rad(self->m_pitch));
	glm_normalize(self->m_dir);

    _calculate_right(self);

    //=======UPDATE VIEW MATRIX=======

    glm_vec3_add(self->m_pos, self->m_dir, self->m_front);

    glm_lookat(self->m_pos, self->m_front, self->m_up, self->m_view);
}