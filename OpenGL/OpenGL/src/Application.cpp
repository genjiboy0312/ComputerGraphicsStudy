#include <GL/glew.h> //glfw보다 먼저 include해야 함
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//	버전 코드
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//	Opengl 3.3 버전 사용
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);		//	Compatability 버전일떄는 VAO를
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		//	Compatability 버전일떄는 VAO를

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기
	//-----Modern OpenGL 방식--------//
	float positions[] = {
		//	앞 3개 Position Data, 뒤 3개 Normal Data
		-0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	0.0f,	//	0
		 0.5f,	-0.5f,	0.0f,	0.0f,	1.0f,	0.0f,	//	1
		 0.5f,	0.5f,	0.0f,	0.0f,	0.0f,	1.0f,	//	2
		-0.5f,	0.5f,	0.0f,	1.0f,	0.0f,	0.0f	//	3
	};

	unsigned int indices[] = {
		0, 1, 2,		//	t1
		2, 3, 0			//	t2
	};

	//	좀 더 고수준 바인딩. Vertex Array Object
	/*unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);*/
	VertexArray va;
	//---------데이터를 전달하는 과정--------//
	VertexBuffer vb{ positions, 4 * 7 * sizeof(float) };

	//	레이아웃에 추가
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	va.AddBuffer(vb, layout);

	IndexBuffer ib{ indices, 6 };

	//	Shader
	Shader shader{ "res/shaders/Basic.shader" };
	shader.Bind();

	Renderer renderer;

	va.UnBind();
	vb.Unbind();
	ib.Unbind();
	shader.Unbind();
	//glUseProgram(shader); //	acitve (bind)

	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glUseProgram(0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		//GLCall(glClear(GL_COLOR_BUFFER_BIT));
		renderer.Clear();

		//va.Bind();			//	glBindVertexArray(vao);
		//shader.Bind();		//	glUseProgram(shader);
		//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));	//	draw call
		renderer.Draw(va, ib, shader);
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}