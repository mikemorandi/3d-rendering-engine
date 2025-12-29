#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

uniform mat4 ModelMatrix;
uniform mat4 LightVPMatrix;

void main()
{
	vec4 posHomogenous = vec4(VertexPosition,1.0);

	gl_Position = LightVPMatrix * ModelMatrix * posHomogenous;
}
