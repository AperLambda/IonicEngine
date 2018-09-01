#version 330 core
layout (location = 0) in vec3 vertex;
out vec2 texCoords;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 transform;
uniform vec2 texCoordsData[4];

void main() {
	gl_Position = projection * (transform * model * vec4(vertex.xy, 0.0, 1.0));
	texCoords = texCoordsData[int(vertex.z)];
}
