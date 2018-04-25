#include "Shader.h"
#include "Renderer.h"

#include<iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>

struct ShaderProgramSources
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSources ParseShader(const std::string& filePath)
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream stream(filePath);
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return{ ss[0].str(), ss[1].str() };
}

Shader::Shader(const std::string& filePath)
	:m_FilePath(filePath), m_RendererID(0)
{
	ShaderProgramSources src = ParseShader(filePath);
	m_RendererID = CreateShader(src.VertexSource, src.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind()
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind()
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, const float v1, const float v2, const float v3, const float v4)
{
	GLCall(glUniform4f(GetUniformLocation(name), v1, v2, v3, v4));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}
	GLCall(int uniformLocation = glGetUniformLocation(m_RendererID, name.c_str()));
	ASSERT(uniformLocation != -1);
	m_UniformLocationCache[name] = uniformLocation;
	return uniformLocation;
}

unsigned int Shader::CompileShader(unsigned int type, std::string &source)
{
	unsigned int shaderId = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shaderId, 1, &src, nullptr);
	glCompileShader(shaderId);

	// Error handling
	int result;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shaderId, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(shaderId);
		return 0;
	}

	return shaderId;
}

unsigned int Shader::CreateShader(std::string &vertexShader, std::string &fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	return program;
}