#version 410

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 PositionData;
layout (location = 2) out vec3 NormalData;


uniform sampler2D PositionTex, NormalTex, ColorTex;

in vec3 Position;
in vec3 Normal;

void main()
{
	PositionData = Position;
	NormalData = Normal;
	FragColor = vec4(1,1,1,1);
}