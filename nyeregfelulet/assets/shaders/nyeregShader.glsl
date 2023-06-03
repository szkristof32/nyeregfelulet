#type vertex
#version 450

layout (location = 0) in vec3 in_position;

layout (location = 0) uniform mat4 projection;
layout (location = 1) uniform mat4 view;

void main()
{
	gl_Position = projection * view * vec4(in_position, 1.0);
}

#type fragment
#version 450

layout (location = 0) out vec4 out_colour;

void main()
{
	out_colour = vec4(1.0, 0.8, 0.3, 1.0);
}