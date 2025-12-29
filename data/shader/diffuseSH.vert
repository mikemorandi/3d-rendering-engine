#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 NormalWs;

uniform mat4 MVP;
uniform mat3 NormalMatrix;

void main() 
{	
	NormalWs = normalize(NormalMatrix * VertexNormal);
	//NormalWs = normalize(VertexNormal);
	gl_Position = MVP * vec4( VertexPosition, 1.0 );
}
