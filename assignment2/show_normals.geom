// Based on tutorial objects_and_stack_geom example.

// Geometry shader to draw normals
// as lines drawn

#version 400 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 model, view, projection; // Model View Projection Matrix

vec3 colour = vec3(1.0, 1.0, 0.0);
float length = 0.2;

in vec3 vnormal[];
out vec3 fcolour;

void main() {
	mat4 mvp = projection * view * model;

	fcolour = colour;

	// v0 is the start of the line, on the vertex position in MVP space
	vec4 v0 = mvp * gl_in[0].gl_Position;

	// Define and emit the vertex
	gl_Position = v0;
	EmitVertex();

	// Calculate the normal vector in MVP space, normalise then scale to specified length
	vec4 normal_mvp = normalize(mvp * vec4(vnormal[0], 0.0)) * length;

	// Specify the other end of the line as the vertex position plus the normal in MVP space
	vec4 v1 = v0 + normal_mvp;

	// define and emit the second vertex
	gl_Position = v1;
	EmitVertex();

	EndPrimitive();
}