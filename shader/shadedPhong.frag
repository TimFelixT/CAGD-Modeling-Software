#version 330 core

smooth in vec3 eyeNormal;
smooth in vec3 eyePosition;

uniform vec4  light;
uniform float lightI;           // Light intensity 
uniform vec3  surfKa;           // Ambient reflectivity
uniform vec3  surfKd;           // Diffuse reflectivity
uniform vec3  surfKs;           // Specular reflectivity
uniform float surfShininess;    // Specular shininess factor

out vec3 fragColor;

// moved from .vert to .frag
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
	vec3 h = (s + v)/length(s + v);

    return LightI * ( 
           Kd * max( dot(s, n), 0.0 ) +
           Ks * pow( max( dot(n,r), 0.0 ), Shininess )) + Ka;
}

void main()
{
  	fragColor = ads(light,  lightI,
			surfKa, surfKd, surfKs, surfShininess,
			eyePosition, eyeNormal);
}

