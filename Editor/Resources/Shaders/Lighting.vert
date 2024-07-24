#version 450

layout(location = 0) out vec2 oTexCoord;

void main()
{
    oTexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

    gl_Position = vec4(oTexCoord.x * 2.f - 1.f, oTexCoord.y * -2.f + 1.f, 0.f, 1.f);
}