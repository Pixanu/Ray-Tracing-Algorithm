#pragma once
#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>
#include "Shape.h"

using namespace glm;



class Plane :public Shape
{
public:
	vec3 planeNormal;

	Plane(void);
	Plane(vec3 _pos, vec3 _normal, vec3 _colour);

	bool Intersection(vec3 rayOrigin, vec3 rayDirection, float& u, float& v, float& w, float* t = NULL);

	vec3 CalculateNormal(vec3 arg_ivan, float& u, float& v, float& w, int* Shininess = NULL, vec3* DiffuseC = NULL, vec3* SpecularC = NULL);

	~Plane();
};


#endif // !PLANE_H 