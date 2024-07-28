#version 450

layout (location = 0) out vec4 oCol;

layout (location = 0) in vec3 iNml;
layout (location = 1) in vec2 iUV;
layout (location = 2) in vec3 iTan;
layout (location = 3) in vec3 iBitan;

layout (set = 1, binding = 0) uniform sampler2D uTex;
layout (set = 1, binding = 1) uniform sampler2D uNormalTex;
layout (set = 1, binding = 2) uniform sampler2D uPBRTex;

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

vec3 EncodeSRGB(vec3 Col)
{
	vec3 Res;

	if (Col.r <= 0.0031308) Res.r = Col.r * 12.92;
	else Res.r = 1.055 * pow(Col.r, 1.0 / 2.4) - 0.055;

	if (Col.g <= 0.0031308) Res.g = Col.g * 12.92;
	else Res.g = 1.055 * pow(Col.g, 1.0 / 2.4) - 0.055;

	if (Col.b <= 0.0031308) Res.b = Col.b * 12.92;
	else Res.b = 1.055 * pow(Col.b, 1.0 / 2.4) - 0.055;

	return clamp(Res, 0.0, 1.0);
}

void main()
{
	vec4 Col = texture(uTex, iUV);
	if (Col.a < .5f) discard;
	Col = vec4(DecodeSRGB(Col.rgb), Col.a);

	mat3 TBN = mat3(iTan, iBitan, iNml);

	vec3 Normal = texture(uNormalTex, iUV).rgb;
	Normal = normalize(Normal * 2.0 - 1.0);
	Normal = normalize(TBN * Normal);

	oCol = vec4(EncodeSRGB(Col.rgb), Col.a);
}