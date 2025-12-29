#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec3 VertexNormal;

out vec3 Normal;

uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
	Normal = NormalMatrix * VertexNormal;
    gl_Position = MVP * vec4( VertexPosition, 1.0 );
}
