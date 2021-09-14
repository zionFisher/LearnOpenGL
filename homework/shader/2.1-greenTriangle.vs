// green triangle vertex shader
#version 330 core

// load attribute from vertices array
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

// pass fscolor to fragmentShader
out vec3 FsColor;

void main()
{
    FsColor = Color;

    // set triangle position
    gl_Position = vec4(Position, 1.0);
}
