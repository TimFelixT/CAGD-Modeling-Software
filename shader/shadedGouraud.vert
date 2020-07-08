#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;  // could be used for diffuse reflectivity

uniform mat4 modelviewMatrix;   // model-view matrix
uniform mat4 projectionMatrix;  // projection matrix
uniform mat3 normalMatrix;      // normal matrix

uniform vec4  light;            // Light position or direction
uniform float lightI;           // Light intensity 
uniform vec3  surfKa;           // Ambient reflectivity
uniform vec3  surfKd;           // Diffuse reflectivity
uniform vec3  surfKs;           // Specular reflectivity
uniform float surfShininess;    // Specular shininess factor

flat          out vec3 fragmentColor;
//noperspective out vec3 fragmentColor;
//smooth        out vec3 fragmentColor;

vec3 ads (vec4 Light, float LightI,
          vec3 Ka, vec3 Kd, vec3 Ks, float Shininess,
	  vec3 Position, vec3 Normal )
{
    vec3 n = normalize(Normal);
    vec3 s = Light.xyz;
    if (Light.w == 1.0) { // positional light
       s = normalize(s - Position);
    } else { // directional light
       s = normalize(s);
    }
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, n );

    return LightI * (
           Kd * max( dot(s, n), 0.0 ) +
           Ks * pow( max( dot(r,v), 0.0 ), Shininess )) + Ka;
}

void main()
{
    vec3 eyePosition = (modelviewMatrix * vec4(position, 1.0)).xyz; // eye-space position
    vec3 eyeNormal   = normalize(normalMatrix * normal);      // eye-space normal 
    gl_Position = (projectionMatrix*modelviewMatrix)* vec4(position, 1.0); // transformed position

    fragmentColor = ads(light, lightI,
			surfKa, surfKd, surfKs, surfShininess,
			eyePosition, eyeNormal);
}
