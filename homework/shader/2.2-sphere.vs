// sphere vertex shader
#version 330 core

// load attribute from vertices array
layout (location = 0) in vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // set triangle position
    gl_Position = projection * view * vec4(Position, 1.0);
}
