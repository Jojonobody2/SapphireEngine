#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 oNml;
layout (location = 1) out vec2 oUV;
layout (location = 2) out vec3 oTan;
layout (location = 3) out vec3 oBitan;

struct Vertex
{
	vec3 Pos;
	float UVx;
	vec3 Nml;
	float UVy;
	vec3 Tan;
	vec3 Bitan;
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
	mat4 ModelView = Uniforms.View * PushConstants.Model;

	Vertex V = PushConstants.pVertexBuffer.Vertices[gl_VertexIndex];
	gl_Position = Uniforms.Proj * ModelView * vec4(V.Pos, 1.0f);

	oUV = vec2(V.UVx, V.UVy);
	oNml = normalize(ModelView * vec4(V.Nml, 0.f)).xyz;
	oTan = normalize(ModelView * vec4(V.Tan, 0.f)).xyz;
	oBitan = normalize(ModelView * vec4(V.Bitan, 0.f)).xyz;
}