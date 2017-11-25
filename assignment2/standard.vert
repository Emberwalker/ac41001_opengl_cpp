// Veretx shader: When completed should implement
// per vertex lighting

// Specify minimum OpenGL version
#version 410 core

// Define the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

// This is the output vertex colour sent to the rasterizer
out vec4 fcolour;
out vec3 N, L, V, frag_position;

// These are the uniforms that are defined in the application
uniform mat4 model, view, projection;
uniform mat3 normalmatrix;
uniform uint colourmode, emitmode, lightmode;
uniform vec4 lightpos;

// Global constants (for this vertex shader)

void main() {
	//vec3 emissive = vec3(0);				// Create a vec3(0, 0, 0) for our emmissive light
	vec4 position_h = vec4(position, 1.0);	// Convert the (x,y,z) position to homogeneous coords (x,y,z,w)
	//vec4 diffuse_albedo;					// This is the vertex colour, used to handle the colourmode change
	vec3 light_pos3 = lightpos.xyz / lightpos.w;

	// Define our vectors to calculate diffuse and specular lighting
	mat4 mv_matrix = view * model;		// Calculate the model-view transformation
	vec4 P = mv_matrix * position_h;	// Modify the vertex position (x, y, z, w) by the model-view transformation
	N = normalize(normalmatrix * normal);		// Modify the normals by the normal-matrix (i.e. to model-view (or eye) coordinates )
	L = light_pos3 - P.xyz;		// Calculate the vector from the light position to the vertex in eye space

	// Calculate the specular component using Phong specular reflection
	V = normalize(-P.xyz);

	fcolour = colour;

    frag_position = position;
	gl_Position = (projection * view * model) * position_h;
}


