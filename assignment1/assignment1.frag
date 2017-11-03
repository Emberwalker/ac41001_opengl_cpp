// Minimal fragment shader

#version 410 core

in vec4 fcolour;
in vec3 N, L, V, frag_position;

out vec4 outputColor;

uniform uint colourmode;
uniform uint emitmode;

// Constants
vec3 global_ambient = vec3(0.05, 0.05, 0.05);
vec3 specular_albedo = vec3(1.0, 0.8, 0.6);
float ambient_multiplier = 0.4;
float attenuation_k = 1.0;
int shininess = 8;

void main() {
    vec4 diffuse_albedo;
    if (colourmode == 1) {
        diffuse_albedo = fcolour;
    } else if (colourmode == 2) {
        diffuse_albedo = vec4(clamp(frag_position, vec3(0.25), vec3(1.0)), 1.0);
    } else {
        diffuse_albedo = vec4(1, 0, 0, 1);
    }

    vec3 ambient = diffuse_albedo.xyz * ambient_multiplier;

    float light_distance = length(L);
    vec3 light_dir = normalize(L);
    vec3 normN = normalize(N);

    vec3 R = reflect(-light_dir, normN);
    vec3 specular = pow(max(dot(R, normalize(V)), 0.0), shininess) * specular_albedo;

    vec3 diffuse = max(dot(normN, light_dir), 0.0) * diffuse_albedo.xyz;
    float attenuation = 1.0 / (1.0 + attenuation_k * pow(light_distance, 2));

    vec3 emissive = vec3(0);
    if (emitmode == 1) emissive = vec3(1, 1, 0.8);

	outputColor = vec4(attenuation * (ambient + diffuse + specular) + emissive + global_ambient, 1.0);
}