#version 410

layout (location = 0) in vec3 VertexPosition;

uniform mat4 LightSpaceMatrix;

void main()
{
	gl_Position = LightSpaceMatrix * vec4(VertexPosition, 1.0);
}
