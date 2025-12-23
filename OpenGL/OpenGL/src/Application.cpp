#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtx/transform.hpp"

//행렬 계산을 위한 GLM 라이브러리 추가
//https://github.com/g-truc/glm
//detail 폴더에 있는 dummy.cpp 프로젝트에서 제외

glm::mat4 GetTranslationMatrix(float tx, float ty, float tz)
{
	glm::mat4 T
	{ 1.0f, 0.0f, 0.0f, tx,
				 0.0f, 1.0f, 0.0f, ty,
				 0.0f, 0.0f, 1.0f, tz,
				 0.0f, 0.0f, 0.0f, 1.0f
	};

	return glm::transpose(T);
}
glm::mat4 GetProjectionMatrix(float fovy, float aspect, float n, float f)
{
	float halfCot = 1 / (glm::tan(fovy / 2));

	glm::mat4 P
	{ halfCot / aspect, 0.0f, 0.0f, 0.0f,
				0.0f, halfCot, 0.0f, 0.0f,
				0.0f, 0.0f, -(f + n) / (f - n), -2 * n * f / (f - n),
				0.0f, 0.0f, -1.0f, 0.0f
	};
	return glm::transpose(P);
}
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Opengl 3.3 버전 사용
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); //1이면 vsync rate와 같은 속도로 화면 갱신

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기

	{

		float positions[] = { //Vertex마다 텍스처 좌표(uv) 데이터 추가
			-0.5f, -0.5f, -5.0f, //0 
			 0.5f, -0.5f, -5.0f, //1
			 0.5f,  0.5f, -5.0f, //2
			-0.5f,  0.5f, -5.0f, //3
		};

		unsigned int indices[] = { //index buffer를 함께 사용(index는 unsigned 타입임에 유의)
			0, 1, 2, //vertex 012로 이루어진 삼각형
			2, 3, 0  //vertex 230로 이루어진 삼각형
		};

		//알파 채널 처리 방법 (chapter 10에서 다룰 예정)
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//vao 생성 VertexArray가 담당
		VertexArray va;
		VertexBuffer vb{ positions, 4 * 3 * sizeof(float) };
		VertexBufferLayout layout;
		layout.Push<float>(3); //vertex당 3개의 위치를 표현하는 float 데이터
		//layout.Push<float>(2); //vertex당 2개의 텍스처 좌표를 표현하는 float 데이터
		va.AddBuffer(vb, layout);

		IndexBuffer ib{ indices, 6 };

		//--------- Model(World) View Projection Matrix 정의 전달 ------//
		//glm::mat4 model = GetTranslationMatrix(1.0, -0.5f, 0.0f);
		glm::mat4 model = GetTranslationMatrix(0.0, 0.0f, 0.0f);
		glm::mat4 proj = GetProjectionMatrix(3.14f / 3, 640 / 480, 1.0f, 100.0f);
		//---------Shader 생성---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		va.UnBind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			shader.Bind();
			shader.SetUniformMat4f("u_Model", model);
			shader.SetUniformMat4f("u_Proj", proj);
			//shader.SetUniformMat4f("u_Model", glm::value_ptr(model));

			//Renderer로 이동
			renderer.Draw(va, ib, shader);

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}


	glfwTerminate();
	return 0;
}