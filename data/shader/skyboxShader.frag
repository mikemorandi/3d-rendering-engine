#version 410

layout (location = 0) out vec4 FragColor;

uniform samplerCube CubeMapTex;

in vec3 VertexPos;

void main()
{
	vec4 cubeMapColor = texture(CubeMapTex,VertexPos);
	FragColor = cubeMapColor;
}