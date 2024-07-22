#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 outColor;

struct Vertex {
	vec3 position;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{	
	//load vertex data from device adress
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	//output data
	gl_Position = vec4(v.position, 1.0f);
	outColor = vec3(1, 1, 1);
}