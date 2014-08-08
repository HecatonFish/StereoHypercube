#include "stdafx.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <glload/gl_3_3.h>
#include <glload/gl_load.hpp>
#include <glutil/glutil.h>
#include <glutil/Shader.h>
#include "framework.h"
#include "drawing.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "fourmath.h"
#include "Clif.h"
#include "data.h"

int g_screen_width;
int g_screen_height;

int roteMode = ROTEMODE_RIGHT;
int renderMode = STEREO;

//Handles to the uniforms for the vertex shader
struct CubeProgramData
{
	GLuint theProgram;

	GLuint modelToWorldTranslate4dUnif;
	GLuint worldToCameraMatrix4dUnif;
	GLuint worldToCameraTranslate4dUnif;
	GLuint viewBoxCenterUnif;
	GLuint viewBoxSideLengthsUnif;
	GLuint frustumTanUnif;

	GLuint modelToWorldMatrixUnif;
	GLuint worldToCameraMatrixUnif;
	GLuint cameraToClipMatrixUnif;
	GLuint baseColorUnif;
};

//To store the shader program handle
CubeProgramData DefaultProgram;

//Handle to the vertex buffer, index buffer and vertex array objects
GLuint cubeVertexBufferObject;
GLuint cubeIndexBufferObject;
GLuint vaoCube;

//R4 Camera Information
static glm::vec4 g_eyePos_4d(0.0f, 0.0f, 0.0f, -8.0f);
static glm::vec4 g_sphereTargRelPos_4d(90.0f, 90.0f, 90.0f, 1.0f);
static glm::vec4 g_toVector_4d(0.0f, 0.0f, 0.0f, 1.0f);

//These clif store the rotors which define the orientation of the 4d object
Clif g_orientation_r_4d;
Clif g_orientation_l_4d;

float rotVel_4d = .2f;

//R3 Camera information
//Recall XY is the screen and Z is out
static glm::vec3 g_camPos_centered(0.0f, 0.0f, -5.0f);
//Relative position of lookat target in spherical coordinates, in degrees. phi theta r
//phi is the flat angle, theta is the azimuthal angle
static glm::vec3 g_sphereTargRelPos(90.0f, 90.0f, 1.0f);
//Towards vector. Used for wasd translation
static glm::vec3 g_toVector(0.0f, 0.0f, 1.0f);
//to x up. Used for wasd translation
static glm::vec3 g_rightVector(1.0f, 0.0f, 0.0f);
//Used for wasd translation
static glm::vec3 g_upVector(0.0f, 1.0f, 0.0f);

//Used for left and right eye offset
static glm::vec3 g_camPos(0.0f, 0.0f, -5.0f);

float g_fzNear = 1.0f;
float g_fzFar = 1000.0f;

//Position of the 4D-world's origin's projection in the 3D space
static glm::vec4 g_originPos(0.0f, 0.0f, 0.0f, 0.0f);

//Handles for the shaders which draw the text overlay
struct TextProgramData
{
	GLuint theProgram;

	GLuint sampler;
	GLuint cameraToClipMatrixUnif;
};

TextProgramData TextProgram;

GLuint textVertexBufferObject;
GLuint vaoText;

std::string coordString;
bool drawText;

//Will need 6 Vertices per character, 2 floats per vertex
std::vector<GLfloat> textVertexData;

glutil::Font *g_pFont = NULL;

//Initializes the shader programs for the thing to be drawn
CubeProgramData InitializeCubeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "FourDModelToClip.vert"));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorDepthCue.frag"));

	CubeProgramData data;
	data.theProgram = Framework::CreateProgram(shaderList);

	data.modelToWorldTranslate4dUnif = glGetUniformLocation(data.theProgram, "modelToWorldTranslate4d");
	data.worldToCameraMatrix4dUnif = glGetUniformLocation(data.theProgram, "worldToCameraMatrix4d");
	data.worldToCameraTranslate4dUnif = glGetUniformLocation(data.theProgram, "worldToCameraTranslate4d");
	data.viewBoxCenterUnif = glGetUniformLocation(data.theProgram, "viewBoxCenterUnif");
	data.viewBoxSideLengthsUnif = glGetUniformLocation(data.theProgram, "viewBoxSideLengths");
	data.frustumTanUnif = glGetUniformLocation(data.theProgram, "frustumTan");

	data.modelToWorldMatrixUnif = glGetUniformLocation(data.theProgram, "modelToWorldMatrix");
	data.worldToCameraMatrixUnif = glGetUniformLocation(data.theProgram, "worldToCameraMatrix");
	data.cameraToClipMatrixUnif = glGetUniformLocation(data.theProgram, "cameraToClipMatrix");
	data.baseColorUnif = glGetUniformLocation(data.theProgram, "baseColor");

	return data;
}

//Initialize shader programs for the text overlay
TextProgramData InitializeTextProgram(){
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "TextRendering.vert"));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorTexture.frag"));

	TextProgramData data;
	data.theProgram = Framework::CreateProgram(shaderList);

	data.cameraToClipMatrixUnif = glGetUniformLocation(data.theProgram, "cameraToClipMatrix");
	data.sampler = glGetUniformLocation(data.theProgram, "texHandle");
	glUseProgram(data.theProgram);
	glUniform1i(data.sampler, 0);
	glUseProgram(0);
	return data;

}

//Initializes the vertex buffer and index buffer for the thing to be drawn
void InitializeCubeVertexBuffer()
{
	glGenBuffers(1, &cubeVertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData)*numCubes, vertexData_Rotated, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &cubeIndexBufferObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Initializes the vertex buffer for the text overlay
void InitializeTextVertexBuffer()
{
	//Each chararacter to be drawn will need 6 vertices, each made up of 2 floats
	// as well as 6 texture coords, each made up of 2 floats.
	//This is 24 floats in total per character.
	//An ample space of 400 characters is reserved.
	glGenBuffers(1, &textVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, textVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*24*400, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Generates the multiplication tables for Clifford algebra elements up to grade 4
void InitializeCliffordTables(){
	std::string clifs[] = { "", "1", "2", "3", "4", "12", "13", "14", "23", "24", "34", "123", "124", "134", "234", "1234" };
	int tempRefTable[16][16];
	int tempSignTable[16][16];

	for (int i = 0; i<16; i++){
		for (int j = 0; j<16; j++){
			std::string product = bladeProduct(clifs[i], clifs[j]);
			tempRefTable[i][j] = Clif::strToRefTable(product.substr(1));
			tempSignTable[i][j] = Clif::charToSignTable(product.at(0));
		}
	}

	Clif::setTables(tempRefTable, tempSignTable);
}

//Calculate Model to Camera transform in 3D
glm::mat4 CalcLookAtMatrix(const glm::vec3 &cameraPt, const glm::vec3 &lookPt, const glm::vec3 &upPt)
{
	glm::vec3 lookDir = glm::normalize(lookPt - cameraPt);
	g_toVector = lookDir;

	glm::vec3 upDir = glm::normalize(upPt);

	glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, upDir));
	g_rightVector = rightDir;

	glm::vec3 perpUpDir = glm::cross(rightDir, lookDir);
	g_upVector = perpUpDir;


	glm::mat4 rotMat(1.0f);
	rotMat[0] = glm::vec4(rightDir, 0.0f);
	rotMat[1] = glm::vec4(perpUpDir, 0.0f);
	rotMat[2] = glm::vec4(-lookDir, 0.0f);

	rotMat = glm::transpose(rotMat);

	glm::mat4 transMat(1.0f);
	transMat[3] = glm::vec4(-cameraPt, 1.0f);

	return rotMat * transMat;
}

//Calculate Model to Camera transform in 4D
//glm only supports matrices up to 4x4, so the translate part is done in the shader via a vec4, instead of the matrix
glm::mat4 CalcLookAtMatrix_4d(const glm::vec4 &cameraPt, const glm::vec4 &lookPt, const glm::vec4 &upPt, const glm::vec4 &overPt){
	glm::vec4 lookDir = glm::normalize(lookPt - cameraPt);
	g_toVector_4d = lookDir;
	glm::vec4 rightDir = glm::normalize(cross4(upPt, overPt, lookDir));
	glm::vec4 upDir = glm::normalize(cross4(overPt, lookDir, rightDir));
	glm::vec4 overDir = glm::normalize(cross4(lookDir, rightDir, upDir));

	glm::mat4 rotMat(1.0f);
	rotMat[0] = rightDir;
	rotMat[1] = upDir;
	rotMat[2] = overDir;
	rotMat[3] = lookDir;

	rotMat = glm::transpose(rotMat);

	return rotMat;
}

//Calculates target position in R3 given Spherical Coordinates relative to camera point
glm::vec3 ResolveTargetPosition(glm::vec3 camPosition)
{
	float phi = Framework::DegToRad(g_sphereTargRelPos.x);
	float theta = Framework::DegToRad(g_sphereTargRelPos.y);

	float fSinTheta = sinf(theta);
	float fCosTheta = cosf(theta);
	float fCosPhi = cosf(phi);
	float fSinPhi = sinf(phi);

	glm::vec3 dirToTarget(fSinTheta * fCosPhi, fCosTheta, fSinTheta * fSinPhi);
	return (dirToTarget * g_sphereTargRelPos.z) + camPosition;
}

//Calculates target position in R4 given Spherical Coordinates relative to camera point
glm::vec4 ResolveTargetPosition_4d(){
	
	float phi_three = Framework::DegToRad(g_sphereTargRelPos_4d[0]);
	float phi_two = Framework::DegToRad(g_sphereTargRelPos_4d[1]);
	float phi_one = Framework::DegToRad(g_sphereTargRelPos_4d[2]);

	float fSinPhiThree = sinf(phi_three);
	float fCosPhiThree = cosf(phi_three);
	float fSinPhiTwo = sinf(phi_two);
	float fCosPhiTwo = cosf(phi_two);
	float fSinPhiOne = sinf(phi_one);
	float fCosPhiOne = cosf(phi_one);

	glm::vec4 dirToTarget_4d(
		fSinPhiOne * fCosPhiTwo,
		fCosPhiOne,
		fSinPhiOne * fSinPhiTwo * fCosPhiThree,
		fSinPhiOne * fSinPhiTwo * fSinPhiThree);

	return (dirToTarget_4d * g_sphereTargRelPos_4d[3]) + g_eyePos_4d;

}

//Applies a translation and the orientation rotor to every element in the vertexData array
//Rotemode is whether the rotor is left or right multiplied with the vertex
void rotateVertices_4d(){
	int floatsPerCube = 4 * numberOfVertices;
	for (int n = 0; n < numCubes; n++){
		for (int i = 0; i < 4*numberOfVertices; i+=4){
			Clif temp(vertexData[i]+offSets[4*n], vertexData[i + 1]+offSets[4*n+1], vertexData[i + 2]+offSets[4*n+2], vertexData[i + 3]+offSets[4*n+3]);
			if (roteMode == ROTEMODE_RIGHT){
				temp = multiplyClifs(g_orientation_l_4d, temp);
				temp = multiplyClifs(temp, g_orientation_r_4d);
			}else{
				temp = multiplyClifs(g_orientation_r_4d, temp);
				temp = multiplyClifs(temp, g_orientation_l_4d);
			}
			vertexData_Rotated[floatsPerCube*n + i] = temp.getCoeff(_E1);
			vertexData_Rotated[floatsPerCube*n + i + 1] = temp.getCoeff(_E2);
			vertexData_Rotated[floatsPerCube*n + i + 2] = temp.getCoeff(_E3);
			vertexData_Rotated[floatsPerCube*n + i + 3] = temp.getCoeff(_E4);
		}
	}
}

//Resets the orientation of the hypercubes
void resetOrientation(){
	for (int i = 0; i < 16; i++)
		g_orientation_r_4d.setCoeff(0, i);
	g_orientation_r_4d.setCoeff(1.0f, _SCALAR);
	g_orientation_l_4d = g_orientation_r_4d.getReverse();
}

//Initializes many of the fields in the program
void init(int width, int height)
{
	g_pFont = glutil::GenerateFont(glutil::FONT_SIZE_MEDIUM);

	DefaultProgram = InitializeCubeProgram();
	TextProgram = InitializeTextProgram();
	InitializeCubeVertexBuffer();
	InitializeTextVertexBuffer();

	//Setup vertex array object for the cube rendering
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBufferObject);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Then setup the vertex array object for the text overlay
	glGenVertexArrays(1, &vaoText);
	glBindVertexArray(vaoText);

	glBindBuffer(GL_ARRAY_BUFFER, textVertexBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	for (int i = 0; i < numCubes; i++)
		cubeRender[i] = false;
	cubeRender[0] = true;
	drawText = true;

	g_screen_width = width;
	g_screen_height = height;

	//Setup the tables for clifford algebra
	InitializeCliffordTables();

	g_orientation_r_4d.setCoeff(1.0f, _SCALAR);
	g_orientation_l_4d = g_orientation_r_4d.getReverse();
}

//Adjust the aspect ratio when the window is resized
void reshape(int w, int h)
{
	glutil::MatrixStack persMatrix;
	if (renderMode == STEREO){
		persMatrix.Perspective(45.0f, ((w/2) / (float)h), g_fzNear, g_fzFar);
	}else if(renderMode == MONO){
		persMatrix.Perspective(45.0f, (w / (float)h), g_fzNear, g_fzFar);
	}
	glUseProgram(DefaultProgram.theProgram);
	glUniformMatrix4fv(DefaultProgram.cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(persMatrix.Top()));
	glUseProgram(0);
}

//The addGlyph() and pushCoords() methods are used for drawing text to the screen
void pushCoords(int vertNum, std::vector<glm::vec2> &pos, std::vector<glm::vec2> &texCoord){
	textVertexData.push_back(pos[vertNum].x);
	textVertexData.push_back(pos[vertNum].y);
	textVertexData.push_back(texCoord[vertNum].x);
	textVertexData.push_back(texCoord[vertNum].y);
}

void addGlyph(const glutil::GlyphQuad &theGlyph){
	std::vector<glm::vec2> positions = theGlyph.GetPositions();
	std::vector<glm::vec2> texCoords = theGlyph.GetTexCoords();

	pushCoords(2, positions, texCoords);
	pushCoords(1, positions, texCoords);
	pushCoords(0, positions, texCoords);

	pushCoords(2, positions, texCoords);
	pushCoords(3, positions, texCoords);
	pushCoords(1, positions, texCoords);
}

//Debug method currently unused
//Displays the contents of a vector on screen
void printVec4(std::stringstream &strStream, std::vector<glutil::GlyphQuad> &glyphs, int &strSize, std::string label, glm::vec4 &vector, int height){
	strStream.str(std::string());
	strStream.clear();
	strStream << std::setprecision(2);
	strStream << label << ": ( ";
	for(int i=0; i<4; i++)
		strStream << vector[i] << ",";
	strStream << ")";
	glyphs.clear();
	glyphs = g_pFont->LayoutLine(strStream.str(), glm::vec2(20.0f, (float)(height)), glutil::REF_BOTTOM);
	for (GLuint i = 0; i < glyphs.size(); i++){
		addGlyph(glyphs[i]);
	}
	strSize += glyphs.size();
}

//Main body of rendering calculations
//If renderMode == STEREO then two images are calculated, each with a slight sideways offset to simulate binocular vision
//if renderMode == MONO then the second image is not drawn, and only the first image is used.

void render(int w, int h, int screen){
	g_screen_width = w;
	g_screen_height = h;
	if (renderMode == STEREO){
		if (screen == 1){
			glViewport(0, 0, (GLsizei)g_screen_width / 2, (GLsizei)g_screen_height);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g_camPos = g_camPos_centered - .007f*g_rightVector;
		}
		else{
			glViewport((GLsizei)g_screen_width / 2, 0, (GLsizei)g_screen_width / 2, (GLsizei)g_screen_height);

			g_camPos = g_camPos_centered + .007f*g_rightVector;
		}
	}
	else if (renderMode == MONO){
		if (screen == 1){
			glViewport(0, 0, (GLsizei)g_screen_width, (GLsizei)g_screen_height);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g_camPos = g_camPos_centered;
		}
		else{
			return;
		}
	}

	glUseProgram(DefaultProgram.theProgram);

	//Pass the 4D projection uniforms to the shader program

	//4D model to world, calculated with rotors
	rotateVertices_4d();
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferObject);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numCubes * 4 * numberOfVertices * sizeof(GLfloat), &vertexData_Rotated[0]);

	//4D World to Camera
	const glm::vec4 &targetPos_4d= ResolveTargetPosition_4d();
	glm::vec4 standardUp(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 standardOver(0.0f, 0.0f, 1.0f, 0.0f);
	glutil::MatrixStack matrix1;
	matrix1.SetMatrix(CalcLookAtMatrix_4d(g_eyePos_4d, targetPos_4d, standardUp, standardOver));//{camera target up over}
	glUniformMatrix4fv(DefaultProgram.worldToCameraMatrix4dUnif, 1, GL_FALSE, glm::value_ptr(matrix1.Top()));

	glUniform4f(DefaultProgram.worldToCameraTranslate4dUnif, -g_eyePos_4d[0], -g_eyePos_4d[1], -g_eyePos_4d[2], -g_eyePos_4d[3]);

	//Calculate projected 4d origin
	g_originPos = matrix1.Top() * -g_eyePos_4d;

	//4D Camera to Clip
	glUniform3f(DefaultProgram.viewBoxCenterUnif, 0.0f, 0.0f, 0.0f);
	glUniform3f(DefaultProgram.viewBoxSideLengthsUnif, 2.0f, 2.0f, 2.0f);

	//4D Clip to Screen
	glUniform1f(DefaultProgram.frustumTanUnif, tanf(Framework::DegToRad(45.0f / 2.0f)));


	//Pass the 3d projection uniforms to the shader program

	//3D World to Camera
	const glm::vec3 &targetPos = ResolveTargetPosition(g_camPos);
	glutil::MatrixStack camMatrix;
	camMatrix.SetMatrix(CalcLookAtMatrix(g_camPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f)));//{camera target up} vectors
	glUniformMatrix4fv(DefaultProgram.worldToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(camMatrix.Top()));

	//3D Model to World
	glutil::MatrixStack modelMatrix;
	glutil::PushStack push(modelMatrix);
	modelMatrix.Scale(glm::vec3(1.0f, 1.0f, 1.0f));
	modelMatrix.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	glUseProgram(DefaultProgram.theProgram);
	glUniformMatrix4fv(DefaultProgram.modelToWorldMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));

	//3D Camera to Screen is calcuated in reshape()

	glLineWidth(3.0f);

	glBindVertexArray(vaoCube);
	for (int i = 0; i < numCubes; i++){
		if (cubeRender[i]){
			//Base Colour Uniform
			glUniform4f(DefaultProgram.baseColorUnif, cubeColors[4 * i], cubeColors[4 * i + 1], cubeColors[4 * i + 2], cubeColors[4 * i + 3]);

			glDrawElementsBaseVertex(GL_LINES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0, i*numberOfVertices);
		}
	}

	glLineWidth(1.0f);

	glBindVertexArray(0);
	glUseProgram(0);



	//Draw the text overlay
	if (drawText){
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(TextProgram.theProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_pFont->GetTexture());

		glutil::MatrixStack persMatrix;
		if (renderMode == MONO)
			persMatrix.PixelPerfectOrtho(glm::ivec2(g_screen_width, g_screen_height), glm::vec2(-1.0f, 1.0f), false);
		else
			persMatrix.PixelPerfectOrtho(glm::ivec2(g_screen_width / 2, g_screen_height), glm::vec2(-1.0f, 1.0f), false);
		glUniformMatrix4fv(TextProgram.cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(persMatrix.Top()));

		textVertexData.clear();
		int strSize = 0;

		std::stringstream strStream;
		strStream << "3D: At ("
			<< std::fixed << std::setprecision(2)
			<< std::setw(6) << g_camPos[0]
			<< ", " << std::setw(6) << g_camPos[1]
			<< ", " << std::setw(6) << g_camPos[2]
			<< ")             Facing <"
			<< std::setw(6) << g_toVector[0]
			<< ", " << std::setw(6) << g_toVector[1]
			<< ", " << std::setw(6) << g_toVector[2]
			<< ">" << std::endl;

		std::vector<glutil::GlyphQuad> glyphs = g_pFont->LayoutLine(strStream.str(), glm::vec2(10.0f, 10.0f), glutil::REF_BOTTOM);
		for (GLuint i = 0; i < glyphs.size(); i++){
			addGlyph(glyphs[i]);
		}
		strSize += glyphs.size();

		strStream.str(std::string());
		strStream.clear();
		strStream << "4D: At ("
			<< std::fixed << std::setprecision(2)
			<< std::setw(6) << g_eyePos_4d[0]
			<< ", " << std::setw(6) << g_eyePos_4d[1]
			<< ", " << std::setw(6) << g_eyePos_4d[2]
			<< ", " << std::setw(6) << g_eyePos_4d[3]
			<< ")     Facing <"
			<< std::setw(6) << g_toVector_4d[0]
			<< ", " << std::setw(6) << g_toVector_4d[1]
			<< ", " << std::setw(6) << g_toVector_4d[2]
			<< ", " << std::setw(6) << g_toVector_4d[3]
			<< ">" << std::endl;

		glyphs.clear();
		glyphs = g_pFont->LayoutLine(strStream.str(), glm::vec2(10.0f, (float)(g_pFont->GetLinePixelHeight() + 15)), glutil::REF_BOTTOM);
		for (GLuint i = 0; i < glyphs.size(); i++){
			addGlyph(glyphs[i]);
		}
		strSize += glyphs.size();

		strStream.str(std::string());
		strStream.clear();
		if (roteMode == ROTEMODE_RIGHT)
			strStream << "Orientation : Right Multiplied";
		else
			strStream << "Orientation : Left Multiplied";

		glyphs.clear();
		glyphs = g_pFont->LayoutLine(strStream.str(), glm::vec2(10.0f, (float)(g_pFont->GetLinePixelHeight() * 2 + 25)), glutil::REF_BOTTOM);
		for (GLuint i = 0; i < glyphs.size(); i++){
			addGlyph(glyphs[i]);
		}
		strSize += glyphs.size();
		//Debug string prints
		/**
		printVec4(strStream, glyphs, strSize, "right ", g_rightVector_4d, 45);
		printVec4(strStream, glyphs, strSize, "up " , g_upVector_4d, 65);
		printVec4(strStream, glyphs, strSize, "over ", g_overVector_4d, 85);
		printVec4(strStream, glyphs, strSize, "to ", g_toVector_4d, 105);
		printVec4(strStream, glyphs, strSize, "angles ", g_sphereTargRelPos_4d, 125);
		**/

		glBindBuffer(GL_ARRAY_BUFFER, textVertexBufferObject);
		glBufferSubData(GL_ARRAY_BUFFER, 0, strSize * 24 * sizeof(GLfloat), &textVertexData[0]);

		glBindVertexArray(vaoText);

		glDrawArrays(GL_TRIANGLES, 0, 6 * strSize);

		glBindVertexArray(0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//Responds to continuous keyboard inputs to rotate or translate the 3D viewpoint and to rotate the 4D object
void update(double delta){
	float d = (float)delta;

	float velTrans = 1.0f;
	float velRot = .1f;
	//Adjust turn rate so that simulteneously turning left and walking right causes the camera to orbit the origin
	//(When you are facing the origin)
	velRot = Framework::RadToDeg(velTrans / glm::max(0.5f, glm::distance(glm::vec2(g_camPos_centered[0], g_camPos_centered[2]), glm::vec2(g_originPos[0], g_originPos[2]))));
	
	if (GetAsyncKeyState(0x57)){//w
		g_camPos_centered[0] += g_toVector[0] * velTrans*d;
		g_camPos_centered[2] += g_toVector[2] * velTrans*d;
	}
	else if (GetAsyncKeyState(0x53)){//s
		g_camPos_centered[0] -= g_toVector[0] * velTrans*d;
		g_camPos_centered[2] -= g_toVector[2] * velTrans*d;
	}
	if (GetAsyncKeyState(0x41)){//a
		g_camPos_centered[0] -= g_rightVector[0] * velTrans*d;
		g_camPos_centered[2] -= g_rightVector[2] * velTrans*d;
	}
	else if (GetAsyncKeyState(0x44)){//d
		g_camPos_centered[0] += g_rightVector[0] * velTrans*d;
		g_camPos_centered[2] += g_rightVector[2] * velTrans*d;
	}
	if (GetAsyncKeyState(0x45)){//e
		g_camPos_centered[1] += velTrans*d;
	}
	else if (GetAsyncKeyState(0x51)){//q
		g_camPos_centered[1] -= velTrans*d;
	}
	
	if (GetAsyncKeyState(VK_LEFT)){
		g_sphereTargRelPos[0] -= velRot*d;
	}
	else if (GetAsyncKeyState(VK_RIGHT)){
		g_sphereTargRelPos[0] += velRot*d;
	}
	if (GetAsyncKeyState(VK_UP)){//i
		g_sphereTargRelPos[1] -= velRot*d;
		if (g_sphereTargRelPos[1] < 1.0f)
			g_sphereTargRelPos[1] = 1.0f;
	}
	else if (GetAsyncKeyState(VK_DOWN)){//k
		g_sphereTargRelPos[1] += velRot*d;
		if (g_sphereTargRelPos[1] > 179.0f)
			g_sphereTargRelPos[1] = 179.0f;
	}
	
	float theta = rotVel_4d * delta;
	Clif orientationChange;//The increment rotor

	if (GetAsyncKeyState(0x52)){//r
		orientationChange.setCoeff(1.0f, _E1E2);
	}
	else if (GetAsyncKeyState(0x46)){//f
		orientationChange.setCoeff(-1.0f, _E1E2);
	}
	if (GetAsyncKeyState(0x54)){//t
		orientationChange.setCoeff(1.0f, _E1E3);
	}
	else if (GetAsyncKeyState(0x47)){//g
		orientationChange.setCoeff(-1.0f, _E1E3);
	}
	if (GetAsyncKeyState(0x59)){//y
		orientationChange.setCoeff(1.0f, _E1E4);
	}
	else if (GetAsyncKeyState(0x48)){//h
		orientationChange.setCoeff(-1.0f, _E1E4);
	}
	if (GetAsyncKeyState(0x55)){//u
		orientationChange.setCoeff(1.0f, _E2E3);
	}
	else if (GetAsyncKeyState(0x4A)){//j
		orientationChange.setCoeff(-1.0f, _E2E3);
	}
	if (GetAsyncKeyState(0x49)){//i
		orientationChange.setCoeff(1.0f, _E2E4);
	}
	else if (GetAsyncKeyState(0x4B)){//k
		orientationChange.setCoeff(-1.0f, _E2E4);
	}
	if (GetAsyncKeyState(0x4F)){//o
		orientationChange.setCoeff(1.0f, _E3E4);
	}
	else if (GetAsyncKeyState(0x4C)){//l
		orientationChange.setCoeff(-1.0f, _E3E4);
	}

	orientationChange.normalize();
	orientationChange.scale(sinf(theta));
	orientationChange.setCoeff(cosf(theta), _SCALAR);

	g_orientation_r_4d = multiplyClifs(g_orientation_r_4d, orientationChange);
	g_orientation_r_4d.normalize();
	g_orientation_l_4d = g_orientation_r_4d.getReverse();

}

//Responds to keyboard input to change program parameters
//Including: which cubes to render, the rendering mode, and the rotation velocity
void processKeyDown(WPARAM w){
	switch (w){
	case '0':
		cubeRender[0] = !cubeRender[0];
		break;
	case '1':  case '2': case '3':case '4':case '5':case '6':case '7': case '8':
		if (w-48 >= 0 && w-48 < numCubes)
			cubeRender[w - 48] = !cubeRender[w - 48];
		break;
	case 'z':
		rotVel_4d -= .1f;
		break;
	case 'c':
		rotVel_4d += .1f;
		break;
	case VK_SPACE:
		resetOrientation();
		break;
	case 0x09://tab
		if (roteMode == ROTEMODE_RIGHT){
			roteMode = ROTEMODE_LEFT;
		}else if(roteMode == ROTEMODE_LEFT){
			roteMode = ROTEMODE_RIGHT;
		}
		break;
	case 'm':
		if (renderMode == STEREO){
			renderMode = MONO;
		}else if (renderMode == MONO){
			renderMode = STEREO;
		}
		reshape(g_screen_width, g_screen_height);
		break;
	case 'n':
		drawText = !drawText;
		break;
	}

}