// Minimal fragment shader

#version 410 core

in vec3 fcolour;
in vec3 N, L, V, P, frag_position;

out vec4 outputColor;

uniform uint colourmode, emitmode, lightmode, fogmode;
uniform vec3 viewpos;
uniform vec4 fogcolour;

// Constants
const vec3 global_ambient = vec3(0.1, 0.1, 0.1);
const vec3 specular_albedo = vec3(0.5, 0.5, 0.5);
const float ambient_multiplier = 0.4;
const float attenuation_k = 0.0001;
const float fog_max = 100.0;
const float fog_min = 1.0;
const float fog_density = 0.9;
const int shininess = 2;

void main() {
    vec3 diffuse_albedo;
    if (colourmode == 1) {
        diffuse_albedo = fcolour;
    } else if (colourmode == 2) {
        diffuse_albedo = vec3(clamp(frag_position, vec3(0.25), vec3(1.0)));
    } else {
        diffuse_albedo = vec3(1, 0, 0);
    }

    vec3 ambient = diffuse_albedo * ambient_multiplier;

    float light_distance = length(L);
    vec3 light_dir = normalize(L);
    vec3 normN = normalize(N);

    vec3 R = reflect(-light_dir, normN);
    vec3 specular = pow(max(dot(R, normalize(V)), 0.0), shininess) * specular_albedo;

    vec3 diffuse = max(dot(normN, light_dir), 0.0) * diffuse_albedo;
    float attenuation = 1.0 / (1.0 + attenuation_k * pow(light_distance, 2));

    vec3 emissive = vec3(0);
    if (emitmode == 1) emissive = vec3(1, 1, 0.8);

    vec4 shaded;
    if (lightmode == 0) {
	    shaded = vec4(attenuation * (ambient + diffuse + specular) + emissive + global_ambient, 1.0);
    } else if (lightmode == 1) {
        shaded = vec4(ambient + diffuse + specular + emissive + global_ambient, 1.0);
    }

    float fog_factor = 1.0;
    float dist = length(P - viewpos);
    if (fogmode == 1) {
        // Linear
        fog_factor = (fog_max - dist) / (fog_max - fog_min);
    } else if (fogmode == 2) {
        // Exponential
        fog_factor = exp(dist * fog_density);
    } else if (fogmode == 3) {
        // Exponential squared
        fog_factor = pow(dist * fog_density, 2.0);
    }
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    if (fogmode == 0) {
        outputColor = shaded;
    } else {
        outputColor = mix(fogcolour, shaded, fog_factor);
    }
}