#version 410 core
// Minimal vertex shader

layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
}