#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define WIDTH 800
#define HEIGHT 600

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

	std::cout << glGetString(GL_VERSION) << std::endl;

	/* Build vertex buffer */
	float posistions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};
	unsigned int vBuffer;
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), posistions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	
	/* Creating shader */
	std::string vertexShader =
		"#version 330 core\n"
		"layout(location = 0) in vec4 position;\n"
		"void main()\n"
		"{\n"
		"\tgl_Position = position;\n"
		"}\n";
	std::string fragmentShader =
		"#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"void main()\n"
		"{\n"
		"\tcolor = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";
	unsigned int shader = CreateShader(vertexShader, fragmentShader);

	/* Using the shader */
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// Test code
		//glBegin(GL_TRIANGLES);
		//glVertex2f(-0.5f, -0.5f);
		//glVertex2f(0.0f, 0.5f);
		//glVertex2f(0.5f, -0.5f);
		//glEnd();

		/* Rendering using vertex buffer */
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}