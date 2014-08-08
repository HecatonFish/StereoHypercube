//A Clif represents a single multivector containing elements of up to grade 4
//Are called "clifs" here because they are elements of the Clifford Algebra

#ifndef _CLIF_H
#define _CLIF_H

#include <string>
#include <iostream>

#define _SCALAR		0
#define _E1			1
#define _E2			2
#define _E3			3
#define _E4			4
#define _E1E2		5
#define _E1E3		6
#define _E1E4		7
#define _E2E3		8
#define _E2E4		9
#define _E3E4		10
#define _E1E2E3		11
#define _E1E2E4		12
#define _E1E3E4		13
#define _E2E3E4		14
#define _E1E2E3E4	15


/**
refTable and signTable are precalculated in init()
They describe how to multiply blades
The row is the left argument
The column is the right argument
The value in refTable is which entry of the coeffs array the result should go to
The value in signTable is the sign of the blade product

ex.
Multiplying e1e4 x e2e4
Go to row _E1E4 (7) and column _E2E4 (9)
refTable[7][9] = 5
signTable[7][9] = -1
Which means e1e4 x e2e4 = -1 * e1e2
(_E1E2 == 5. The e1e2 coefficient of a clif is stored in coeffs[5])
**/

class Clif
{
public:
	static int refTable[16][16];
	static int signTable[16][16];

	float coeffs[16];

	Clif();
	Clif(float coeffsIn[16]);
	Clif(float v1, float v2, float v3, float v4);//vector style initialization

	//Utility methods used in generating the tables
	static void setTables(int refIn[16][16], int signIn[16][16]);
	static int strToRefTable(std::string str);
	static int charToSignTable(char c);

	void setCoeff(float val, int index);
	void addToCoeff(float val, int index);
	float getCoeff(int index);

	//Returns a Clif with all its elements reversed: ex. e1e2 --> e2e1
	Clif getReverse();

	void scale(float f);
	void normalize();

	friend Clif multiplyClifs(Clif left, Clif right);

};

Clif multiplyClifs(Clif left, Clif right);

void printClif(std::string, Clif c);

#endif