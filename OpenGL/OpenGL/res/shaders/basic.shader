#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

void main()
{
	gl_Position = position;		//	gl_Position에 넣어야 할 정보 : Clip space coordinate
	v_Color = a_Color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;
in vec4 v_Color;

void main()
{
	//color = vec4(1.0, 0.0 ,0.0, 1.0);
	// color = u_Color;
	color = v_Color;
};