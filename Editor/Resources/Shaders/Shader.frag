#version 450

layout (location = 0) out vec4 oColor;

layout (location = 0) in vec3 iColor;

void main()
{
	oColor = vec4(iColor, 1.f);
}