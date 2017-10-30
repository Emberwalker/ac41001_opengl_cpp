// Minimal vertex shader

#version 410 core

// These are the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

// Uniform variables are passed in from the application
uniform mat4 model, view, projection;
uniform mat3 n_matrix;
uniform uint colourmode;

// Output the vertex colour - to be rasterized into pixel fragments
out vec4 fcolour;

void main()
{
	vec4 diffuse_colour;
	vec4 position_h = vec4(position, 1.0);
	
	if (colourmode == 1)
		diffuse_colour = colour;
	else
		diffuse_colour = vec4(0.0, 1.0, 0, 1.0);

    vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);

    mat4 mv = view * model;
    vec3 light_dir = normalize(mat3(mv) * vec3(1, 1 ,1));

    vec3 N = normalize(n_matrix * normal);

    float diffuse_component = dot(light_dir, N);
    diffuse_component = max(diffuse_component, 0.0);

	//===--- Phong
	float shininess = 0.2;
	vec4 specular_colour = diffuse_colour;

	vec3 V = normalize(-(mv * position_h).xyz);
	vec3 R = reflect(-light_dir, N);
	vec4 specular = pow(max(dot(R, V), 0.0), shininess) * specular_colour;

	// Define the vertex colour
	fcolour = (ambient + diffuse_component + specular) * diffuse_colour;

	// Define the vertex position
	gl_Position = projection * view * model * position_h;
}

