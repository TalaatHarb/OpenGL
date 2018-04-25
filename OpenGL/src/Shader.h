#pragma once
#include<iostream>
#include<unordered_map>

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::string& filePath);
	~Shader();
	void Bind();
	void UnBind();

	// Set uniforms
	void SetUniform4f(const std::string& name, const float v1, const float v2, const float v3, const float v4);

private:
	int GetUniformLocation(const std::string& name);
	unsigned int CompileShader(unsigned int type, std::string &source);
	unsigned int CreateShader(std::string &vertexShader, std::string &fragmentShader);
};

