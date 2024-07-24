#version 450

layout (location = 0) out vec4 oCol;

layout (location = 0) in vec3 iNml;
layout (location = 1) in vec2 iUV;

layout (set = 1, binding = 0) uniform sampler2D uTex;

void main()
{
	oCol = texture(uTex, iUV);
}