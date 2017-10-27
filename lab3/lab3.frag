// Minimal fragment shader

#version 410 core

in vec4 fcolour;
out vec4 outputColor;
void main()
{
	outputColor = fcolour;
}