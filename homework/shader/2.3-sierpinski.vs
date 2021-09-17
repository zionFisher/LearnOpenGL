#version 330

layout(location = 0)in vec3 Position; 
layout(location = 1)in vec3 Color; 
out vec4 color; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(Position, 1.0); 
	color = vec4(Color, 1.0);
}