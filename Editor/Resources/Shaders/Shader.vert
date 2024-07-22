#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 oColor;

struct Vertex
{
	vec3 Pos;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer
{
	Vertex Vertices[];
};


layout( push_constant ) uniform constants
{
	mat4 MVPMat;
	VertexBuffer pVertexBuffer;
} PushConstants;

void main() 
{	
	Vertex V = PushConstants.pVertexBuffer.Vertices[gl_VertexIndex];
	gl_Position = PushConstants.MVPMat * vec4(V.Pos, 1.0f);

	oColor = vec3(0.3215686274509804f, 0.66862745098039217f, 0.2784313725490196f);
}