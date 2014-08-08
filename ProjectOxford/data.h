/**
This file contains all the numerical data for the program.

offSets[] and cubeColors[] contain the positional offset and the colours of all the cubes to be rendered
vertexData[] contains the vertices for a single hypercube. There are 16 vertices in a hypercube.
vertexData_rotated will store the vertices for all 9 hypercubes after they have been rotated.
indexData[] contains the information for the edges of a hypercube - which vertices link to which vertices

**/

#ifndef _DATA_H
#define _DATA_H

////// Object Data Begin - Adjacency Visualizer /////////////////////////////////////////////////

const float shift = 2.0f;
const int numCubes = 9;

bool cubeRender[numCubes];

const float offSets[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	shift, 0.0f, 0.0f, 0.0f,
	-shift, 0.0f, 0.0f, 0.0f,
	0.0f, shift, 0.0f, 0.0f,
	0.0f, -shift, 0.0f, 0.0f,
	0.0f, 0.0f, shift, 0.0f,
	0.0f, 0.0f, -shift, 0.0f,
	0.0f, 0.0f, 0.0f, shift,
	0.0f, 0.0f, 0.0f, -shift
};


#define BROWN  .53f, .27f, .24f,  1.0f
#define YELLOW .88f, .81f, .39f,  1.0f
#define DAWN   .48f, .31f, .40f,  1.0f
#define DUSK   .84f, .03f, .32f,  1.0f
#define GREEN  .28f, .50f, .30f,  1.0f
#define BLUE   .11f, .38f, .37f,  1.0f
#define ORANGE .98f, .41f, 0.0f,  1.0f
#define POWDER .41f, .82f, .91f,  1.0f 
#define WHITE  1.0f, 1.0f, 1.0f,  1.0f
#define BLACK  0.0f, 0.0f, 0.0f,  1.0f


const float cubeColors[] = {
	WHITE,
	BROWN,
	YELLOW,
	DAWN,
	DUSK,
	GREEN,
	BLUE,
	ORANGE,
	POWDER
};


const int numberOfVertices = 16;

const float vertexData[] = {
	//Hypercube vertices
	-1.0f, -1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 1.0f,
};

float vertexData_Rotated[numCubes * numberOfVertices * 4];

const GLshort indexData[] =
{
	0, 1,
	1, 2,
	2, 3,
	3, 0,

	4, 5,
	5, 6,
	6, 7,
	7, 4,

	0, 4,
	1, 5,
	2, 6,
	3, 7,

	8, 9,
	9, 10,
	10, 11,
	11, 8,

	12, 13,
	13, 14,
	14, 15,
	15, 12,

	8, 12,
	9, 13,
	10, 14,
	11, 15,

	0, 8,
	1, 9,
	2, 10,
	3, 11,
	4, 12,
	5, 13,
	6, 14,
	7, 15,
};

////// Object Data End /////////////////////////////////////////////////

#endif