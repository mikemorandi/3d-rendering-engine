#version 410

struct PointLight
{
	vec4 Position;
	vec3 Color;
};

struct SpotLight
{
	vec4 Position;
	vec3 Color;
	vec3 Direction;
	float CutoffAngle;
};


layout (std140) uniform Lights
{
	PointLight PointLights[4];
	SpotLight SpotLights[4];
} pLights;

uniform vec3 MaterialColor;	//diffuse reflectivity
uniform int NumPointLights;
uniform int NumSpotLights;

in vec3 Position;
in vec3 Normal;
in vec3 LightIntensity;

layout (location = 0) out vec4 FragColor;

void main()
{

	vec3 LightIntensity = vec3(0);

	for(int i=0; i < NumSpotLights; i++)
	{
		SpotLight spot = pLights.SpotLights[i];
		
		vec3 s = normalize( vec3(spot.Position) - Position );

		float angle = acos( dot(-s,spot.Direction) );
		float cutoff = radians( clamp (spot.CutoffAngle, 0.0, 90.0) ) ;

		if( angle  < cutoff)
		{
			LightIntensity += spot.Color * MaterialColor * max( dot( s, Normal ), 0.0 );
		}
	}

	FragColor = vec4(LightIntensity.xyz, 1.0);
}