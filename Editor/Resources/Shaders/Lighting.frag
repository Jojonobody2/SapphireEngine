#version 450

layout(location = 0) in vec2 iTexCoord;

layout(location = 0) out vec4 oCol;

layout(set = 0, binding = 0) uniform sampler2D uAlbedoImage;
//layout(set = 0, binding = 1) uniform sampler2D uDepthImage;

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

mat3 AcesInputMatrix = mat3
    (
        0.59719f, 0.35458f, 0.04823f,
        0.07600f, 0.90834f, 0.01566f,
        0.02840f, 0.13383f, 0.83777f
    );

mat3 AcesOutputMatrix = mat3
    (
        1.60475f, -0.53108f, -0.07367f,
       -0.10208f,  1.10813f, -0.00605f,
       -0.00327f, -0.07276f,  1.07602f
    );

void main()
{
    vec3 Col = EncodeSRGB(texture(uAlbedoImage, iTexCoord).rgb);

    vec3 V = Col * AcesInputMatrix;
    vec3 A = V * (V + 0.0245786f) - 0.000090537f;
    vec3 B = V * (0.983729f * V + 0.4329510f) + 0.238081f;
    V = A / B;

    oCol = vec4(V * AcesOutputMatrix, 1.f);
}