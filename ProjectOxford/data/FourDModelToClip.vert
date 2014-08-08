#version 330

layout(location = 0) in vec4 position;

//4D projection constants
uniform vec4 modelToWorldTranslate4d;
uniform mat4 worldToCameraMatrix4d;
uniform vec4 worldToCameraTranslate4d;
uniform vec3 viewBoxCenter;
uniform vec3 viewBoxSideLengths;
uniform float frustumTan;

//3D projection constants
uniform mat4 cameraToClipMatrix;
uniform mat4 worldToCameraMatrix;
uniform mat4 modelToWorldMatrix;

//For the fragment shader to depthcue
out float colorScale;

void main()
{
	//4d projection
	//remember to clip w=0
	vec4 temp = modelToWorldTranslate4d + position;
	temp = temp + worldToCameraTranslate4d;
	temp = worldToCameraMatrix4d * temp;

	//Scale color by w eye-coordinate
	colorScale = 1.0f-min((temp[3]-8.0f)/2.5f, .5f);

	//if(temp[3]!=0.0f)
	temp = temp / (frustumTan * temp[3]);
	temp[3] = 1.0f;

	temp.xyz = viewBoxCenter + (viewBoxSideLengths/2.0f)*temp.xyz;
	
	//3d projection
	temp = modelToWorldMatrix * temp;
	temp = worldToCameraMatrix * temp;
	gl_Position = cameraToClipMatrix * temp;
}
