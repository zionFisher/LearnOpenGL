// green triangle fragment shader
#version 330 core

in vec3 FsColor;

// pass triangle color to render pipeline
out vec4 TriangleColor;

void main()
{
    TriangleColor = vec4(FsColor, 1.0f);
}