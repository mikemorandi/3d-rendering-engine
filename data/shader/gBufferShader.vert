#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	vec4 posHomogenous = vec4(VertexPosition,1.0);

	Position = vec3(ModelViewMatrix * posHomogenous);
	Normal = normalize(NormalMatrix * VertexNormal);

	gl_Position = MVP * posHomogenous;
}
