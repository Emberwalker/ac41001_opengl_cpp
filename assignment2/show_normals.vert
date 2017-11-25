// Based on tutorial objects_and_stack_geom example.

// Veretx shader to use to draw object showing vertex normals
// as lines drawn

#version 400

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

out vec3 vnormal;

void main() {
	vnormal = normal;

	gl_Position = vec4(position, 1.0);
}