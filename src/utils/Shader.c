#include "Shader.h"

#include <stdio.h>
#include <stdlib.h>

GLint _compile_shader(const char* shader_path, GLenum type)
{
	FILE* file;

	errno_t error_code;

	error_code = fopen_s(&file, shader_path, "rb");

	if (file == NULL)
	{
		printf("%s\n", shader_path);
		perror("file not found\n");
		return 0;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	GLchar* buffer = (GLchar*)malloc(size + 1);

	size_t bytes_read = fread(buffer, 1, size, file);

	if (bytes_read != size)
	{
		perror("failed to read file\n");
		free(buffer);
		fclose(file);
		return 0;
	}

	// null termination
	buffer[size] = '\0';

	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar * const*)&buffer, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::COMPILATION_FAILED\n %s", infoLog);
		return 0;
	}

	free(buffer);

	return shader;
}