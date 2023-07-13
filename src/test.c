#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>
#include <stb_image_loader/stb_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "utils/Constants.h"
#include "utils/Camera.h"
#include "utils/Shader.h"

// how much the mouse moved along the screen
vec2 mouse_delta = {0.0f, 0.0f};
vec2 mouse_pos = {0.0f, 0.0f};

bool first_frame = true;

bool window_resized = false;

bool paused = false;

bool wireframe_enabled = false;

bool esc_pressed = false;

bool tab_pressed = false;

bool left_pressed = false;
bool right_pressed = false;
bool space_pressed = false;
bool left_shift_pressed = false;
bool up_pressed = false;
bool down_pressed = false;

static void _framebuffer_callback(GLFWwindow* window, int width, int height);

static void _mouse_curser_callback(GLFWwindow* window, double x_pos, double y_pos);

static void _process_input(GLFWwindow *window);

static inline void _update_mouse_position();

static GLuint _load_texture(const char* file_path);

int main(int argc, char **argv)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Lighting Scene", NULL, NULL);

	if (window == NULL)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glfwSetFramebufferSizeCallback(window, _framebuffer_callback);
	glfwSetCursorPosCallback(window, _mouse_curser_callback);

	glfwSwapInterval(1);
	
	glViewport(0, 0, 800, 800);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	//======== compile shaders ========


	//===GET SHADER FILE PATH===
	char shader_path[256] = {"\0"};

	strcat(shader_path, argv[0]);
	strcat(shader_path, "/../res/shaders");


	//====CREATE_BASIC_SHADER====
	GLuint basic_shader_program = glCreateProgram();

	char basic_vert_shader_path[256];

	strcpy(basic_vert_shader_path, shader_path);
	strcat(basic_vert_shader_path, "/basic.vert");

	GLuint vert_shader = _compile_shader(basic_vert_shader_path, GL_VERTEX_SHADER);

	glAttachShader(basic_shader_program, vert_shader);

	char basic_frag_shader_path[256];

	strcpy(basic_frag_shader_path, shader_path);
	strcat(basic_frag_shader_path, "/basic.frag");


	GLuint frag_shader = _compile_shader(basic_frag_shader_path, GL_FRAGMENT_SHADER);

	glAttachShader(basic_shader_program, frag_shader);

	glLinkProgram(basic_shader_program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);


	//====CREATE LIGHTING SCENE SHADER====
	GLuint lighting_shader_program = glCreateProgram();

	char lighting_vert_shader_path[256];

	strcpy(lighting_vert_shader_path, shader_path);
	strcat(lighting_vert_shader_path, "/scene.vert");

	GLuint lighting_vert_shader = _compile_shader(lighting_vert_shader_path, GL_VERTEX_SHADER);

	glAttachShader(lighting_shader_program, lighting_vert_shader);

	char lighting_frag_shader_path[256];

	strcpy(lighting_frag_shader_path, shader_path);
	strcat(lighting_frag_shader_path, "/scene.frag");


	GLuint lighting_frag_shader = _compile_shader(lighting_frag_shader_path, GL_FRAGMENT_SHADER);

	glAttachShader(lighting_shader_program, lighting_frag_shader);

	glLinkProgram(lighting_shader_program);

	glDeleteShader(lighting_vert_shader);
	glDeleteShader(lighting_frag_shader);



	//=======LOAD BOX TEXTURE=======

	//===GET TEXTURE FILE PATH===
	char texture_path[256] = {"\0"};

	strcat(texture_path, argv[0]);
	strcat(texture_path, "/../res/textures");

	GLuint container_texture, specular_map;
	{
		char container_texture_file_path[256];
		strcpy(container_texture_file_path, texture_path);
		strcat(container_texture_file_path, "/container2.png");

		char specular_map_path[256];
		strcpy(specular_map_path, texture_path);
		strcat(specular_map_path, "/specular_map.png.png");

		container_texture = _load_texture(container_texture_file_path);
		specular_map = _load_texture(specular_map_path);
	}


	// position, color, normal, tex_coords

	float vertices[] = {
		// back - negative z
		0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		// front - positive z
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		// left - negative x
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		// right - positive x
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		// bottom - negative y
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

		// top - positive y
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,

		4, 5, 7,
		5, 6, 7,

		8, 9, 11,
		9, 10, 11,

		12, 13, 15,
		13, 14, 15,

		16, 17, 19,
		17, 18, 19,

		20, 21, 23,
		21, 22, 23,
	};

	GLuint VAO, VBO, IBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int stride = 11 * sizeof(float);

	// vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// used for the lighting scene
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// surface normals attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// texture coordinates attribute
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);



	vec3 light_color = {1.0f, 1.0f, 1.0f};
	vec3 object_color = {1.0f,0.5f, 0.31f};

	

	glUseProgram(basic_shader_program);

	GLint u_proj_loc = glGetUniformLocation(basic_shader_program, "projection");
	GLint u_view_loc = glGetUniformLocation(basic_shader_program, "view");
	GLint u_model_loc = glGetUniformLocation(basic_shader_program, "model");

	GLint u_color_loc = glGetUniformLocation(basic_shader_program, "uColor");
	glUniform3f(u_color_loc, light_color[0], light_color[1], light_color[2]);





	glUseProgram(lighting_shader_program);

	GLint u_scene_proj_loc = glGetUniformLocation(lighting_shader_program, "projection");
	GLint u_scene_view_loc = glGetUniformLocation(lighting_shader_program, "view");
	GLint u_scene_model_loc = glGetUniformLocation(lighting_shader_program, "model");


	GLint u_light_color_loc = glGetUniformLocation(lighting_shader_program, "lightColor");
	glUniform3f(u_light_color_loc, 0.0f, 1.0f, 1.0f);
	GLint u_object_color_loc = glGetUniformLocation(lighting_shader_program, "objectColor");
	glUniform3f(u_object_color_loc, 1.0f, 0.5f, 0.31f);
	GLint u_light_pos_loc = glGetUniformLocation(lighting_shader_program, "lightPos");
	glUniform3f(u_light_pos_loc, 1.2f, 1.0f, 2.0f);

	GLint u_material_diffuse_loc = glGetUniformLocation(lighting_shader_program, "material.diffuse");
	glUniform1i(u_material_diffuse_loc, 0);

	GLint u_material_specular_loc = glGetUniformLocation(lighting_shader_program, "material.specular");
	glUniform1i(u_material_specular_loc, 1);

	GLint u_material_shininess_loc = glGetUniformLocation(lighting_shader_program, "material.shininess");
	glUniform1f(u_material_shininess_loc, 256.0f);

	vec3 diffuse_color; glm_vec3_mul(light_color, (vec3){0.5f, 0.5f, 0.5f}, diffuse_color);
	vec3 ambient_color; glm_vec3_mul(diffuse_color, (vec3){0.2f, 0.2f, 0.2f}, ambient_color);

	GLint u_light_ambient_loc = glGetUniformLocation(lighting_shader_program, "light.ambient");
	glUniform3f(u_light_ambient_loc, ambient_color[0], ambient_color[1], ambient_color[2]);
	GLint u_light_diffuse_loc = glGetUniformLocation(lighting_shader_program, "light.diffuse");
	glUniform3f(u_light_diffuse_loc, diffuse_color[0], diffuse_color[1], diffuse_color[2]);
	GLint u_light_specular_loc = glGetUniformLocation(lighting_shader_program, "light.specular");
	glUniform3f(u_light_specular_loc, 1.0f, 1.0f, 1.0f);

	GLint u_light_view_pos_loc = glGetUniformLocation(lighting_shader_program, "viewPos");

	glUseProgram(0);


	float camera_speed = 10.0f;
	Camera cam; init_camera(&cam);
	vec3 relative_translation= {0.0f, 0.0f, 0.0f};


	float light_move_radius = 7.0f;
	float light_z_pos, light_y_pos;



	mat4 cube_model = GLM_MAT4_IDENTITY_INIT;
	glm_translate(cube_model, (vec3){0.0f, 0.0f, 0.0f});
	glm_scale(cube_model, (vec3){1.0f, 1.0f, 1.0f});

	mat4 light_model = GLM_MAT4_IDENTITY_INIT;
	glm_translate(light_model, (vec3){1.2f, 1.0f, 2.0f});
	glm_scale(light_model, (vec3){0.2f, 0.2f, 0.2f});


	int screen_width, screen_height, aspect_ratio;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);

	float time = glfwGetTime(), prev_time = time;
	float delta_time;

	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_process_input(window);

		time = glfwGetTime();
		delta_time = time - prev_time;
		prev_time = time;

		if (window_resized)
		{
			glfwGetFramebufferSize(window, &screen_width, &screen_height);
			init_projection(&cam, screen_width/(float)screen_height, 45.0f, 0.1f, 100.0f);
		}


		if (!paused)
		{
			relative_translation[DIR_FORWARD] = (up_pressed - down_pressed) * camera_speed * delta_time;
			relative_translation[DIR_SIDE] = (right_pressed - left_pressed) * camera_speed * delta_time;
			relative_translation[DIR_VERTICAL] = (space_pressed - left_shift_pressed) * camera_speed * delta_time;

			update_camera(&cam, mouse_delta, relative_translation);

			
			light_color[0] = sinf(time * 2.0f);
			light_color[1] = sinf(time * 0.7f);
			light_color[2] = sinf(time * 1.3f);

			glUseProgram(basic_shader_program);

			//glUniform3f(u_color_loc, light_color[0], light_color[1], light_color[2]);


			glm_vec3_mul(light_color, (vec3){0.5f, 0.5f, 0.5f}, diffuse_color);
			glm_vec3_mul(diffuse_color, (vec3){0.2f, 0.2f, 0.2f}, ambient_color);

			glUseProgram(lighting_shader_program);

			//glUniform3f(u_light_ambient_loc, ambient_color[0], ambient_color[1], ambient_color[2]);
			//glUniform3f(u_light_diffuse_loc, diffuse_color[0], diffuse_color[1], diffuse_color[2]);
		}


		glUseProgram(basic_shader_program);

		light_z_pos = light_move_radius * cosf(time) + 1.0f;
		light_y_pos = light_move_radius * sinf(time) + 1.0f;

		//glm_translate(light_model, (vec3){0.0f, light_y_pos, light_z_pos});

		glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, (float*)cam.m_proj);
		glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, (float*)cam.m_view);
		glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, (float*)light_model);


		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

		//glm_translate(light_model, (vec3){-0.0f, -light_y_pos, -light_z_pos});



		glUseProgram(lighting_shader_program);

		glUniformMatrix4fv(u_scene_proj_loc, 1, GL_FALSE, (float*)cam.m_proj);
		glUniformMatrix4fv(u_scene_view_loc, 1, GL_FALSE, (float*)cam.m_view);

		glUniformMatrix4fv(u_scene_model_loc, 1, GL_FALSE, (float*)cube_model);

		glUniform3f(u_light_view_pos_loc, cam.m_pos[DIR_X], cam.m_pos[DIR_Y], cam.m_pos[DIR_Z]);

		//glUniform3f(u_light_pos_loc, 0.0f, light_y_pos, light_z_pos);
		glUniform3f(u_light_pos_loc, 1.2f, 1.0f, 2.0f);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container_texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular_map);

		glBindVertexArray(lightVAO);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);



		_update_mouse_position();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteProgram(basic_shader_program);
	glDeleteProgram(lighting_shader_program);

	glfwDestroyWindow(window);
	glfwTerminate();
	
    return 0;
}



static void _framebuffer_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	window_resized = true;
}

static void _mouse_curser_callback(GLFWwindow* window, double x_pos, double y_pos)
{
	if (first_frame)
	{
		mouse_pos[DIR_X] = x_pos;
		mouse_pos[DIR_Y] = y_pos;

		first_frame = false;
	}

	mouse_delta[DIR_X] = x_pos - mouse_pos[DIR_X];
	mouse_delta[DIR_Y] = y_pos - mouse_pos[DIR_Y];
}

static void _process_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!esc_pressed && !paused)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			paused = true;
		}
		else if (!esc_pressed && paused)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			paused = false;
		}
		esc_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
	{
		esc_pressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (!tab_pressed && !wireframe_enabled)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe_enabled = true;
		}
		else if (!tab_pressed && wireframe_enabled)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe_enabled = false;
		}
		tab_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
	{
		tab_pressed = false;
	}

	//=======FORWARD TRANSLATION=======
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		up_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
	{
		up_pressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		down_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
	{
		down_pressed = false;
	}

	//=======SIDEWAYS_TRANSLATION=======
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		left_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
	{
		left_pressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		right_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
	{
		right_pressed = false;
	}

	//=======VERTICAL TRANSLATION
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		space_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		space_pressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		left_shift_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		left_shift_pressed = false;
	}
}

static inline void _update_mouse_position()
{
	glm_vec2_add(mouse_pos, mouse_delta, mouse_pos);
	mouse_delta[DIR_X] = 0.0f; mouse_delta[DIR_Y] = 0.0f;
}

static GLuint _load_texture(const char* file_path)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int tex_width, tex_height, nr_channels;
	unsigned char *data = stbi_load(file_path, &tex_width, &tex_height, &nr_channels, 0);
	if (data)
	{
		GLenum format;
		switch(nr_channels) {
			case 1:
			format = GL_RED;
			break;
			case 3:
			format = GL_RGB;
			break;
			case 4:
			format = GL_RGBA;
			break;
			default:
			format = GL_RGB;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("%s\n", "Failed to load texture");
	}
	stbi_image_free(data);
}