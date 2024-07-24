#version 450

layout (location = 0) out vec4 oCol;

layout (location = 0) in vec3 iNml;
layout (location = 1) in vec2 iUV;

layout (set = 1, binding = 0) uniform sampler2D uTex;

vec3 DecodeSRGB(vec3 Col)
{
	vec3 Res;

	if (Col.r <= 0.04045) Res.r = Col.r / 12.92;
	else Res.r = pow((Col.r + 0.055) / 1.055, 2.4);

	if (Col.g <= 0.04045) Res.g = Col.g / 12.92;
	else Res.g = pow((Col.g + 0.055 ) / 1.055, 2.4);

	if (Col.b <= 0.04045) Res.b = Col.b / 12.92;
	else Res.b = pow((Col.b + 0.055 ) / 1.055, 2.4);

	return clamp(Res, 0.0, 1.0);
}

void main()
{
	vec4 Col = texture(uTex, iUV);
	if (Col.a < .5f) discard;

	oCol = vec4(DecodeSRGB(Col.rgb), Col.a);
}