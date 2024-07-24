#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 oNml;
layout (location = 1) out vec2 oUV;

struct Vertex
{
	vec3 Pos;
	float UVx;
	vec3 Nml;
	float UVy;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer
{
	Vertex Vertices[];
};


layout( push_constant ) uniform constants
{
	mat4 Model;
	VertexBuffer pVertexBuffer;
} PushConstants;


layout (set = 0, binding = 0) uniform UBO
{
	mat4 View;
	mat4 Proj;
} Uniforms;

void main() 
{	
	Vertex V = PushConstants.pVertexBuffer.Vertices[gl_VertexIndex];
	gl_Position = Uniforms.Proj * Uniforms.View * PushConstants.Model * vec4(V.Pos, 1.0f);

	oNml = V.Nml;
	oUV = vec2(V.UVx, V.UVy);	
}