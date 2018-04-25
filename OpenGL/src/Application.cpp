#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define WIDTH 800
#define HEIGHT 600

#include "Renderer.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

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
	{
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

		// Vertex array object
		VertexArray vArrayObject;

		// Vertex buffer
		VertexBuffer vBuffer(posistions, 4 * 2 * sizeof(float));

		// Vertex buffer layout using attributes
		VertexBufferLayout vBufferLayout;
		vBufferLayout.Push<float>(2);

		vArrayObject.AddBuffer(vBuffer, vBufferLayout);

		// Index buffer object
		IndexBuffer iBufferObject(indices, 6);

		/* Creating shader */
		Shader shader("res/shaders/Basic.shader");

		/* Using the shader */
		shader.Bind();

		/* Setting uniforms */
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		// Unbind everything
		vArrayObject.UnBind();
		vBuffer.UnBind();
		iBufferObject.UnBind();
		shader.UnBind();

		float r = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			// Bind
			vArrayObject.Bind();
			iBufferObject.Bind();
			shader.Bind();

			/* Setting Uniforms */
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

			/* Rendering using vertex buffer */
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			// Unbind
			shader.UnBind();
			iBufferObject.UnBind();
			vArrayObject.UnBind();

			/* Animate the color */
			if (r > 1.0 || r < 0.0)
				increment = -increment;
			r += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}