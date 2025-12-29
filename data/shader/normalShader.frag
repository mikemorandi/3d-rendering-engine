#version 410

in vec3 Normal;

out vec4 fragColor;

void main()
{
	vec3 n = normalize(Normal);
	fragColor = vec4(n, 1.0);
}