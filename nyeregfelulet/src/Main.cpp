#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef _WIN32
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#include <iostream>
#include <cassert>

#include "NyeregGenerator.h"
#include "Shader.h"

int main()
{
	setlocale(LC_ALL, "");

	int success = glfwInit();
	assert(success && "GLFW inicializáció nem sikerült...");

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(1600, 900, "Nyeregfelület", nullptr, nullptr);
#ifdef _WIN32
	SetWindowTextA(glfwGetWin32Window(window), "Nyeregfelület");
#endif
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, (vidmode->width - 1600) / 2, (vidmode->height - 900) / 2);

	glfwMakeContextCurrent(window);
	success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	assert(success && "Glad inicializáció nem sikerült...");

	NyeregGenerator generator(10);

	Shader shader("assets/shaders/nyeregShader.glsl");

	glm::mat4 projection = glm::perspective(glm::radians(70.0f), 1600.0f / 900.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		shader.Start();
		shader.LoadMat4("projection", projection);
		shader.LoadMat4("view", view);

		generator.GetVertexArray().Bind();
		generator.GetIndexBuffer().Bind();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawElements(GL_TRIANGLES, generator.GetVertexCount(), GL_UNSIGNED_INT, nullptr);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		generator.GetVertexArray().Unbind();
		generator.GetIndexBuffer().Unbind();

		shader.Stop();

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}