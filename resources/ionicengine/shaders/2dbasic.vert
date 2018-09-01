#version 330 core
in vec2 vertex;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 transform;

void main()
{
    gl_Position = projection * (transform * model * vec4(vertex.xy, 0.0, 1.0));
}