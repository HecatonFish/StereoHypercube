/**
Performs two calculations:
1. The 4d analogue of a cross product
This is the hodge dual of the wedge product of three input vectors.
It is calculated using a determinant-like equation.

2. The product of two basis elements in a Clifford algebra.
A basis element ei * ej is represented as a string "ij", for example
The scalar is an empty string
The string is manipulated until it matches one of the standard forms listed in Clif.h
This lets us generate the lookup tables to later calculate products of Clifs (multivectors)

All use the Euclidean metric
**/

#ifndef _FOURMATH_H
#define _FOURMATH_H

#include <glm/glm.hpp>
#include <string>

glm::vec4 cross4(const glm::vec4 &,const glm::vec4 &, const glm::vec4 &);
std::string bladeProduct(std::string left, std::string right);

#endif