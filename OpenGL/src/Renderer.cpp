#include "Renderer.h"

#include <GL/glew.h>
#include<iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLCallLog(const char* function, const char* file, int line)
{
	if (GLenum error = glGetError())
	{
		std::cout << "[OpenGL error] (" << error << ")" << " " << function << " -> " << file << ":" << line << std::endl;
		while (error = glGetError())
			std::cout << "[OpenGL error] (" << error << ")" << " " << function << " -> " << file << ":" << line << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}