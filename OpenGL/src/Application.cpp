#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define WIDTH 800
#define HEIGHT 600

// Compiler dependent assertion macro
#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCall(x)	GLClearError();\
					x;\
					ASSERT(GLCallLog(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLCallLog(const char* function, const char* file, int line)
{
	if (GLenum error = glGetError())
	{
		std::cout << "[OpenGL error] (" << error << ")" << " " << function << " -> " << file << ":" << line << std::endl;
		while(error = glGetError())
			std::cout << "[OpenGL error] (" << error << ")" << " " << function << " -> " << file << ":" << line << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}

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
	return{ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, std::string &source)
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
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex":"fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(shaderId);
		return 0;
	}

	return shaderId;
}

static unsigned int CreateShader(std::string &vertexShader, std::string &fragmentShader) 
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

int main(void)
{
	GLFWwindow* window;

	/* Initialize GLFW */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Center window */
	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, (vidMode->width - WIDTH) / 2, (vidMode->height - HEIGHT) / 2);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Intialize GLEW */
	if (glewInit() != GLEW_OK)
		return -1;

	/* Enabling V-Sync */
	glfwSwapInterval(1);

	std::cout << glGetString(GL_VERSION) << std::endl;

	/* Build vertex buffer */
	float posistions[] = {
		-0.5f, -0.5f,	//0
		 0.5f, -0.5f,	//1
		 0.5f, 0.5f,	//2
		-0.5f, 0.5f		//3
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0
	};
	unsigned int vBuffer;
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 *2* sizeof(float), posistions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	/* Creating shader */
	std::string vertexShader;
	std::string fragmentShader;
	ShaderProgramSources src = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(src.VertexSource, src.FragmentSource);

	/* Using the shader */
	glUseProgram(shader);

	/* Setting uniforms */
	GLCall(int uniformLocation = glGetUniformLocation(shader, "u_Color"));
	ASSERT(uniformLocation != -1);
	GLCall(glUniform4f(uniformLocation, 0.2f, 0.3f, 0.8f, 1.0f));

	float r = 0.0f;
	float increment = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Setting Uniforms */
		GLCall(glUniform4f(uniformLocation, r, 0.3f, 0.8f, 1.0f));

		/* Rendering using vertex buffer */
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		/* Animate the color */
		if (r > 1.0 || r < 0.0)
			increment = -increment;
		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}