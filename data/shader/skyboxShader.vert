#version 410

layout (location = 0) in vec3 VertexPosition;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform int SkyboxSize;

out vec3 VertexPos;

void main()
{
	VertexPos = VertexPosition;

	mat4 viewM = ViewMatrix;
	//Elliminate the translatonal part of the view matrix, see chapter 19 of GPU gems
	viewM[3] = vec4(0,0,0,1);

    gl_Position = ProjectionMatrix  * viewM * vec4( VertexPosition * SkyboxSize, 1.0 );	
}
