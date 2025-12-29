#version 410

uniform sampler2D DepthTex;
uniform float NearPlane;
uniform float FarPlane;
uniform bool Perspective;

in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - z * (FarPlane - NearPlane));
}


void main() 
{
	float depth = texture(DepthTex,TexCoord).r;

	if(Perspective)
		depth = linearizeDepth(depth) / FarPlane;

    FragColor = vec4(vec3(depth), 1.0); // perspective
}
