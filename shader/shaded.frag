#version 330 core

flat          in vec3 fragmentColor;
//noperspective in vec3 fragmentColor;
//smooth        in vec3 fragmentColor;

out vec3 fragColor;

void main()
{
	fragColor = fragmentColor;
}

