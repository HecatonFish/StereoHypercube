#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 texCoord;

smooth out vec2 glyphCoord;

uniform mat4 cameraToClipMatrix;

void main(){
	gl_Position = cameraToClipMatrix * position;
	glyphCoord = texCoord.st;
}