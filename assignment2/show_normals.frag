// Based on tutorial objects_and_stack_geom example.

// Fragment shader to draw normals
// as lines drawn

#version 400 core

out vec4 colour;

in vec3 fcolour;

void main() {
	colour = vec4(fcolour, 1.0);
}