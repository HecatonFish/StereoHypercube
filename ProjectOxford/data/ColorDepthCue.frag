#version 330

uniform vec4 baseColor;

in float colorScale;

out vec4 outputColor;

void main()
{
	outputColor = baseColor*colorScale;
}
