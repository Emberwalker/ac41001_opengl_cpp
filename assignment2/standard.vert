// Veretx shader: When completed should implement
// per vertex lighting

// Specify minimum OpenGL version
#version 410 core

// Define the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

// Outputs
out vec3 fcolour;
out vec3 N, L, V, P, frag_position;

// Application uniforms
uniform mat4 model, view, projection;
uniform mat3 normalmatrix;
uniform uint colourmode, emitmode, lightmode, fogmode;
uniform vec4 lightpos;

// Global constants (for this vertex shader)
const vec3 gamma = vec3(1.0/2.2);

void main() {
	vec4 position_h = vec4(position, 1.0);	// Convert the (x,y,z) position to homogeneous coords (x,y,z,w)
	vec3 light_pos3 = lightpos.xyz / lightpos.w;

	mat4 mv_matrix = view * model;

	// Modify the vertex position (x, y, z, w) by the model-view transformation
	vec4 P4 = mv_matrix * position_h;
	P = P4.xyz / P4.w;

	// Modify the normals by the normal matrix
	N = normalize(normalmatrix * normal);

	// Calculate the vector from the light position to the vertex in eye space
	L = light_pos3 - P;

	// Calculate the specular component using Phong specular reflection
	V = normalize(-P);

    // Gamma correction
    // See https://www.tomdalling.com/blog/modern-opengl/07-more-lighting-ambient-specular-attenuation-gamma/
	fcolour = pow(colour.rgb, gamma);

    frag_position = position;
	gl_Position = (projection * view * model) * position_h;
}


