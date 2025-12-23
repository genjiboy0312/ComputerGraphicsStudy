#shader vertex
#version 330

layout(location = 0) in vec4 position; 

uniform mat4 u_Model;
uniform mat4 u_Proj;
void main()
{
	gl_Position = u_Proj * u_Model * position; // 정점 위치를 투영 행렬과 곱
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0, 0.0, 0.0, 1.0);
};