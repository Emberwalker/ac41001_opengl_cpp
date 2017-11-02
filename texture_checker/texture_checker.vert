// Basic vertex shader with per-vertex lighting (Gourauyd shading)
// Iain Martin 2017

#version 400

// These are the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texcoord;

// Uniform variables are passed in from the application
uniform mat4 model, view, projection;
uniform uint colourmode;

// Output the vertex colour - to be rasterized into pixel fragments
out vec4 fcolour;
out vec2 ftexcoord;
vec4 ambient = vec4(0.2, 0.2,0.2,1.0);
vec3 light_dir = vec3(0.0, 0.0, 10.0);

void main()
{
	vec4 specular_colour = vec4(1.0,1.0,1.0,1.0);
	vec4 diffuse_colour = vec4(0.5,0.5,0,1.0);
	vec4 position_h = vec4(position, 1.0);
	float shininess = 8.0;
	
	if (colourmode == 1)
		diffuse_colour = vec4(1.0, 1.0, 1.0, 1.0);
	else
		diffuse_colour = vec4(0.0, 1.0, 0, 1.0);

	ambient = diffuse_colour * 0.2;

	mat4 mv_matrix = view * model;

	// It's more efficient to calculate this in your application
	mat3 n_matrix = transpose(inverse(mat3(mv_matrix)));  
	vec3 N = normalize(n_matrix * normal);
	light_dir = normalize(light_dir);

	vec4 diffuse = max(dot(N, light_dir), 0.0) * diffuse_colour;

	vec4 P = position_h * mv_matrix;
	vec3 half_vec = normalize(light_dir + P.xyz);
	vec4 specular = pow(max(dot(N, half_vec), 0.0), shininess) * specular_colour;

	// Define the vertex colour
	fcolour = diffuse + ambient + specular;

	// Define the vertex position
	gl_Position = projection * view * model * position_h;

	// Output the texture coordinates
	ftexcoord = texcoord.xy;
}

