/**
The main part of the calculations.
See comments in the .cpp file for details
**/

#ifndef _DRAWING_H
#define _DRAWING_H

#include <glm/glm.hpp>
#include <glutil/glutil.h>
#include <sstream>
#include <vector>

#define ROTEMODE_LEFT 1
#define ROTEMODE_RIGHT 2

#define STEREO 1
#define MONO 2

struct CubeProgramData;
struct TextProgramData;

CubeProgramData InitializeCubeProgram();
TextProgramData InitializeTextProgram();
void InitializeCubeVertexBuffer();
void InitializeTextVertexBuffer();
void InitializeCliffordTables();
glm::mat4 CalcLookAtMatrix(const glm::vec3 &cameraPt, const glm::vec3 &lookPt, const glm::vec3 &upPt);
glm::mat4 CalcLookAtMatrix_4d(const glm::vec4 &cameraPt, const glm::vec4 &lookPt, const glm::vec4 &upPt, const glm::vec4 &overPt);
glm::vec3 ResolveTargetPosition(glm::vec3);
glm::vec4 ResolveTargetPosition_4d();
void rotateVertices_4d();
void resetOrientation();

//Text rendering functions
void pushCoords(int vertNum, std::vector<glm::vec2> &pos, std::vector<glm::vec2> &texCoord);
void addGlyph(const glutil::GlyphQuad &theGlyph);
void printVec4(std::stringstream &strStream, std::vector<glutil::GlyphQuad> &glyphs, int &strSize, std::string label, glm::vec4 &vector, int height);

void init(int width, int height);
void reshape(int w, int h);
void render(int w, int h, int s);
void update(double delta);

void processKeyDown(WPARAM);

#endif