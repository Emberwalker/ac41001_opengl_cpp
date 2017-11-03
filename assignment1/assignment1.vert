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
uniform uint colourmode, emitmode;
uniform vec4 lightpos;

// Global constants (for this vertex shader)

void main() {
	//vec3 emissive = vec3(0);				// Create a vec3(0, 0, 0) for our emmissive light
	vec4 position_h = vec4(position, 1.0);	// Convert the (x,y,z) position to homogeneous coords (x,y,z,w)
	//vec4 diffuse_albedo;					// This is the vertex colour, used to handle the colourmode change
	vec3 light_pos3 = lightpos.xyz / lightpos.w;

	// Switch the vertex colour based on the colourmode
	/*if (colourmode == 1)
		diffuse_albedo = colour;
	else
		diffuse_albedo = vec4(1.0, 0, 0, 1.0);*/

	//vec3 ambient = diffuse_albedo.xyz *0.2;

	// Define our vectors to calculate diffuse and specular lighting
	mat4 mv_matrix = view * model;		// Calculate the model-view transformation
	vec4 P = mv_matrix * position_h;	// Modify the vertex position (x, y, z, w) by the model-view transformation
	N = normalize(normalmatrix * normal);		// Modify the normals by the normal-matrix (i.e. to model-view (or eye) coordinates )
	L = light_pos3 - P.xyz;		// Calculate the vector from the light position to the vertex in eye space
	//float distanceToLight = length(L);	// For attenuation
	//L = normalize(L);					// Normalise our light vector
	
	// Calculate the diffuse component
	//vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo.xyz;

	// Calculate the specular component using Phong specular reflection
	V = normalize(-P.xyz);
	//vec3 R = reflect(-L, N);
	//vec3 specular = pow(max(dot(R, V), 0.0), shininess) * specular_albedo;

	// Calculate the attenuation factor;
	//float attenuation_k = 1.0;
    //float attenuation = 1.0 / (1.0 + attenuation_k * pow(distanceToLight, 2));

	// If emitmode is 1 then we enable emmissive lighting
	//if (emitmode == 1) emissive = vec3(1.0, 1.0, 0.8);

	// Calculate the output colour, includung attenuation on the diffuse and specular components
	// Note that you may want to exclude the ambient form the attenuation factor so objects
	// are always visible, or include a global ambient
	//fcolour = vec4(attenuation*(ambient + diffuse + specular) + emissive + global_ambient, 1.0);
	fcolour = colour;

	gl_Position = (projection * view * model) * position_h;
}


