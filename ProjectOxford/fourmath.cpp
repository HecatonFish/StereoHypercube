#include "stdafx.h"
#include <glm/glm.hpp>
#include "fourmath.h"

glm::vec4 cross4(const glm::vec4 &u, const glm::vec4 &v, const glm::vec4 &w){

	float A, B, C, D, E, F;	/* Intermediate Values */
	glm::vec4 temp;			/* Intermediate Vector */

	A = (v[0] * w[1]) - (v[1] * w[0]);
	B = (v[0] * w[2]) - (v[2] * w[0]);
	C = (v[0] * w[3]) - (v[3] * w[0]);
	D = (v[1] * w[2]) - (v[2] * w[1]);
	E = (v[1] * w[3]) - (v[3] * w[1]);
	F = (v[2] * w[3]) - (v[3] * w[2]);

	temp[0] = (u[1] * F) - (u[2] * E) + (u[3] * D);
	temp[1] = -(u[0] * F) + (u[2] * C) - (u[3] * B);
	temp[2] = (u[0] * E) - (u[1] * C) + (u[3] * A);
	temp[3] = -(u[0] * D) + (u[1] * B) - (u[2] * A);

	return temp;
}


std::string bladeProduct(std::string left, std::string right){
	char tempChar;
	int coeff = 1;
	std::string tempStr = left + right;

	bool swapped = false;
	//Loop until no more swaps - the string is sorted

	do{
		swapped = false;

		//tempStr.length() returns a uint type, so cast to int. Otherwise get integer underflow
		for (int i = 0; i<int(tempStr.length()) - 1; i++){
			if (tempStr[i] > tempStr[i + 1]){
				tempChar = tempStr[i];
				tempStr[i] = tempStr[i + 1];
				tempStr[i + 1] = tempChar;
				swapped = true;
				coeff *= -1;
				break;
			}
		}

		//Check for e_i * e_i = 1
		//If so, remove it from the expression, since it has a product of 1
		for (int i = 0; i < int(tempStr.length()) - 1; i++){
			if (tempStr[i] == tempStr[i + 1]){
				if (tempStr.length() == 2){
					tempStr = "";
					break;
				}
				else{
					tempStr = (tempStr.substr(0, i) + tempStr.substr(i + 2));
					i--;
				}
			}
		}

	} while (swapped);

	if (coeff == 1)
		tempStr = std::string("+") + tempStr;
	else if (coeff == -1)
		tempStr = std::string("-") + tempStr;

	return tempStr;

}
