#version 330 core

layout (location = 0) in vec2 pos;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(pos.x, pos.y, 0.0, 1.0);
}