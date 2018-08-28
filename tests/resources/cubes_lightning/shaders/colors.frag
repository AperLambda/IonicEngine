// Credits to learnopengl.com

#version 330 core
out vec4 FragColor;

uniform vec4 objectColor;
uniform vec4 lightColor;

void main()
{
    FragColor = lightColor * objectColor;
}