#version 330 core
in vec2 texCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 inColor;

void main()
{
    color = inColor * texture(image, texCoords);
}