#ifndef SPHERE_H
#define Sphere_H


#pragma once
#include <glm/glm.hpp>
#include "Shape.h"


using namespace std;
using namespace glm;

class Sphere : public Shape
{
public:
	vec3 position;
	vec3 colour;
	float radius;

	Sphere(void);

	Sphere(vec3 _pos, float _rad, vec3 _colour);

	virtual bool Intersection(vec3 rayOrigin, vec3 rayDirection, float& u, float& v, float& w, float* t = NULL);

	virtual vec3 CalculateNormal(vec3 arg_ivan, float& u, float& v, float& w, int* Shininess = NULL, vec3* DiffuseC = NULL, vec3* SpecularC = NULL);

	~Sphere();





};

#endif // !SPHERE_H

