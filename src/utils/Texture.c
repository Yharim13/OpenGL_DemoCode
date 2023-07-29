
#include "Texture.h"

#include <stb_image_loader/stb_image.h>

#include <stdio.h>

GLuint load_texture(const char* file_path) {
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