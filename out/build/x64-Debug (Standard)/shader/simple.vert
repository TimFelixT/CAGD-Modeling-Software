#version 330 core

in vec3 position;
in vec3 color;

uniform mat4 mvp;

out vec3 fragmentColor;

void main()
{
	fragmentColor = color;
	gl_Position   = mvp * vec4(position,  1.0);
}