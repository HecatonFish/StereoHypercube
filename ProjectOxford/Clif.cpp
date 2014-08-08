#include "stdafx.h"
#include "Clif.h"
#include <cmath>

int Clif::signTable[16][16] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

int Clif::refTable[16][16] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

Clif::Clif(){
	for (int i = 0; i<16; i++){
		coeffs[i] = 0.0f;
	}
}

Clif::Clif(float coeffsIn[16]){
	for (int i = 0; i<16; i++){
		coeffs[i] = coeffsIn[i];
	}
}

Clif::Clif(float v1, float v2, float v3, float v4){
	for (int i = 0; i<16; i++){
		coeffs[i] = 0.0f;
	}
	coeffs[_E1] = v1;
	coeffs[_E2] = v2;
	coeffs[_E3] = v3;
	coeffs[_E4] = v4;
}

void Clif::setTables(int refIn[16][16], int signIn[16][16]){
	for (int i = 0; i<16; i++){
		for (int j = 0; j<16; j++){
			refTable[i][j] = refIn[i][j];
			signTable[i][j] = signIn[i][j];
		}
	}
}

int Clif::strToRefTable(std::string str){
	if (str == ""){
		return _SCALAR;
	}
	else if (str == "1"){
		return _E1;
	}
	else if (str == "2"){
		return _E2;
	}
	else if (str == "3"){
		return _E3;
	}
	else if (str == "4"){
		return _E4;
	}
	else if (str == "12"){
		return _E1E2;
	}
	else if (str == "13"){
		return _E1E3;
	}
	else if (str == "14"){
		return _E1E4;
	}
	else if (str == "23"){
		return _E2E3;
	}
	else if (str == "24"){
		return _E2E4;
	}
	else if (str == "34"){
		return _E3E4;
	}
	else if (str == "123"){
		return _E1E2E3;
	}
	else if (str == "124"){
		return _E1E2E4;
	}
	else if (str == "134"){
		return _E1E3E4;
	}
	else if (str == "234"){
		return _E2E3E4;
	}
	else if (str == "1234"){
		return _E1E2E3E4;
	}
	return 0;
}

int Clif::charToSignTable(char c){
	if (c == '+'){
		return 1;
	}
	else if (c == '-'){
		return -1;
	}
	return 1;
}

void Clif::setCoeff(float val, int index){
	coeffs[index] = val;
}

void Clif::addToCoeff(float val, int index){
	coeffs[index] += val;
}

float Clif::getCoeff(int index){
	return coeffs[index];
}

//Adjust the sign of each component, as if they were reversed
//ex. ij -> ji = -ij. So put a -1 there.
//Notice that ijk = s kji, where s is +/- 1
//Then (ijk) kji = (s kji) kji
//			1        = s (kji)^2
//Which is why we can just look up s as the diagonal of signTable
Clif Clif::getReverse(){
	Clif temp;
	for (int i = 0; i < 16; i++){
		temp.setCoeff(coeffs[i] * signTable[i][i], i);
	}
	return temp;
}

void Clif::normalize(){
	float mag = 0.0f;
	for (int i = 0; i<16; i++){
		mag += (coeffs[i] * coeffs[i]);
	}
	
	float sqrt_mag = sqrt(mag);

	if (mag != 0){
		for (int i = 0; i < 16; i++){
			coeffs[i] /= sqrt_mag;
		}
	}
}

void Clif::scale(float scale){
	for (int i = 0; i < 16; i++){
		coeffs[i] *= scale;
	}
}

Clif multiplyClifs(Clif left, Clif right){
	Clif answer;

	int tempResultIdx;
	float tempResult;
	//Iterate over the coefficients of the left clif
	for (int i = 0; i<16; i++){
		if (left.getCoeff(i) != 0){
			//Iterate over the coefficients of the right clif
			for (int j = 0; j<16; j++){
				tempResult = left.getCoeff(i) * right.getCoeff(j) * (float)Clif::signTable[i][j];
				tempResultIdx = Clif::refTable[i][j];
				answer.addToCoeff(tempResult, tempResultIdx);
			}
		}
	}

	return answer;
}

void printClif(std::string str, Clif c){
	std::cout << str << " ";
	for (int i = 0; i<16; i++){
		std::cout << c.getCoeff(i) << " ";
	}
	std::cout << std::endl;
}