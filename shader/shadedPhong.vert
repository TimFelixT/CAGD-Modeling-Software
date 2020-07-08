#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;   // could be used as diffuse reflectivity

uniform mat4 modelviewMatrix;    // model-view matrix
uniform mat4 projectionMatrix;   // projection matrix
uniform mat3 normalMatrix;       // normal matrix

smooth out vec3 eyePosition;
smooth out vec3 eyeNormal;

void main()
{
        eyePosition = (modelviewMatrix * vec4(position, 1.0)).xyz; // eye-space position
	eyeNormal   = normalize(normalMatrix * normal);      // eye-space normal 
	gl_Position = (projectionMatrix*modelviewMatrix) * vec4(position, 1.0); // projected position
}