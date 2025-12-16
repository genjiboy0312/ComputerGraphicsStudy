#include <GL/glew.h> //glfw보다 먼저 include해야 함
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
// 이번에는 Vertex Array와 Shader를 이용하여 삼각형을 그리는 Modern OpenGL 방식으로 구현할 것임
// Vertex Array는 GPU의 VRAM에 Buffer에 저장되는 데이터를 넘기는 방식을 이야기함
// 데이터를 넘기고 나서 삼각형을 실제로 그리는 명령을 호출하는 것을 Draw call이라고 함
// 삼각형이 어떻게 그려질지 사용자가 구현한 프로그램이 Shader임

// OpenGL은 State Machine과 비슷함. 일반적인 객체 지향 프로그램의 설계와는 다르게,
// 현재 처리할 데이터를 선택한 후, 현재 실행해야 할 작업을 함수 호출을 통해 처리하는 방식임
// 간단하게 두 삼각형을 화면에 그리는 의사코드로 설명하면,

// ---객체지향 방식
// Triangle t1, t2; //삼각형 두 개를 정의
// Draw(t1); //t1 객체를 전달함으로써 삼각형 1을 그림
// Draw(t2); //t2 객체를 전달함으로써 삼각형 2를 그림

// ---State Machine 방식
// Triangle t1, t2; //삼각형 두 개를 정의
// Activate(t1); //삼각형 1을 처리중인 상태로 설정
// Draw(); //현재 처리중인 데이터(=삼각형 1)를 화면에 그림
// Activate(t2); //삼각형 2를 처리중인 상태로 설정
// Draw(); //현재 처리중인 데이터(=삼각형 2)를 화면에 그림

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragSource;
};

//--------Shader 컴파일 함수----------//
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type); //셰이더 객체 생성(마찬가지)
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); // 셰이더의 소스 코드 명시
	glCompileShader(id); // id에 해당하는 셰이더 컴파일

	// Error Handling(없으면 셰이더 프로그래밍할때 괴롭다...)
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); //셰이더 프로그램으로부터 컴파일 결과(log)를 얻어옴
	if (result == GL_FALSE) //컴파일에 실패한 경우
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log의 길이를 얻어옴
		char* message = (char*)alloca(length * sizeof(char)); //stack에 동적할당
		glGetShaderInfoLog(id, length, &length, message); //길이만큼 log를 얻어옴
		std::cout << "셰이더 컴파일 실패! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id); //컴파일 실패한 경우 셰이더 삭제
		return 0;
	}

	return id;
}

//--------Shader 프로그램 생성, 컴파일, 링크----------//
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader)
{
	unsigned int program = glCreateProgram(); //셰이더 프로그램 객체 생성(int에 저장되는 것은 id)
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

	//컴파일된 셰이더 코드를 program에 추가하고 링크
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//셰이더 프로그램을 생성했으므로 vs, fs 개별 프로그램은 더이상 필요 없음
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos) //vertex 셰이더 섹션
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) //fragment 셰이더 섹션
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; //코드를 stringstream에 삽입
		}
	}
	return { ss[0].str(), ss[1].str() };
}
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

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기
	//-----Modern OpenGL 방식--------//
	// http://docs.gl/ 에서 아래 gl함수들의 reference를 찾을 수 있음

	float positions[] = { //삼각형 좌표 정보. 현재는 CPU side memory에 있음
		-0.5f, -0.5f, 0.0f,		//	0
		 0.5f,  -0.5f,0.0f,		//	1
		 0.5f, 0.5f,0.0f,		//	2
		 -0.5f, 0.5f,0.0f,		//	3
	};

	unsigned int indices[] = {
		0, 1, 2,		//	t1
		2, 3, 0			//	t2
	};

	unsigned int ibo;		//	Index Buffer Object
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);		//	바인딩
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//---------데이터를 전달하는 과정--------//
	unsigned int bufferID;
	glGenBuffers(1, &bufferID); //1. 데이터를 저장할 버퍼 객체를 만들고(1개), 그 버퍼 객체의 주소를 bufferID에 저장
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //2. 방금 만든 buffer(두 번째 인자로 주소를 넘겨준)를 "작업 상태"로 만듬
	// 첫 번째 GL_ARRAY_BUFFER는 버퍼에 배열 데이터가 저장될 것이라는 의미
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), positions, GL_STATIC_DRAW);   //3. 이제 실제로 GPU에 데이터를 넘겨주는 함수를 호출 

	//	데이터를 해석하는 방법
	glEnableVertexAttribArray(0);

	//	3 부분 : 하나의 vertex에 몇 개의 데이터를 넘기는지
	//	sizeof(float) * 3 : 만큼 건너 뛰라는 것
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//---------Shader 생성---------------//
	//std::string vertexShader =
	//	"#version 330 core\n"
	//	"\n"
	//	"layout(location = 0) in vec4 position;" //여기 있는 location = 0가, 118, 119 line의 0을 의미함
	//	"\n"
	//	"void main()\n"
	//	"{\n"
	//	"	gl_Position = position;\n" //119에서 보다시피, 2개의 값만 전달했지만, 알아서 vec4로 변환해줌
	//	"}\n";

	//std::string fragShader =
	//	"#version 330 core\n"
	//	"\n"
	//	"layout(location = 0) out vec4 color;" //출력 color
	//	"\n"
	//	"void main()\n"
	//	"{\n"
	//	"	color = vec4(1.0, 0.0 ,0.0, 1.0);\n" //빨간색 반환
	//	"}\n";

	//unsigned int shader = CreateShader(vertexShader, fragShader);
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); //셰이더 코드 파싱
	unsigned int shader = CreateShader(source.VertexSource, source.FragSource);
	glUseProgram(shader); //BindBuffer와 마찬가지로, 현재 셰이더 프로그램을 "작업 상태"로 놓음
	//draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		// 삼각형 그리는 Legacy 코드 추가 <-- 예전 방식임.
				//glBegin(GL_TRIANGLES);
				//glVertex2f(-0.5f, -0.5f);
				//glVertex2f( 0.0f,  0.5f);
				//glVertex2f( 0.5f, -0.5f);
				//glEnd();
		//glUseProgram(0);		//	deactivate -> 다시 흰색으로 나옴
				//----------Modern OpenGL----------//
				// 주의해서 알아야 할 점 : 아래 draw call에서, 삼각형을 그리는 것은
				// 66번째 line에서 glBindBuffer()가 되어 있기 때문임.
				// 아래 glDrawArray에서는 무엇을 그릴 것인지 인자로 넘기는 것이 아니라,
				// 현재 "작업 상태"에 들어와 있는 대상을 그리는 것
		//glDrawArrays(GL_TRIANGLES, 0, 3);				//실제 draw call, 삼각형을 그릴 것이라고 명시
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);	//	draw call -> 사각형

		//현재는 아무것도 화면에 나오지 않을 것임. 
		//왜냐하면 삼각형을 어떻게 그릴 것인지 쉐이더를 통해 알려주지 않았기 때문!

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);			//	셰이더 삭제
	glfwTerminate();
	return 0;
}