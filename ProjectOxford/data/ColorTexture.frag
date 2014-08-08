#version 330

smooth in vec2 glyphCoord;
out vec4 outputColor;

uniform sampler2D texHandle;

void main(){
	outputColor = texture(texHandle, glyphCoord).rrrr;
}