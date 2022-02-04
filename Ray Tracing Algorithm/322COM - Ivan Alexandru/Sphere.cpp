#include "Sphere.h"

#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

Sphere::Sphere(void)
{
	position = vec3(0, 0, 0);
	radius = 0;
	colour = vec3(0, 0, 0);
}

Sphere::Sphere(vec3 _pos, float _rad, vec3 _colour)
{
	position = _pos;
	radius = _rad;
	colour = _colour;
}

bool Sphere::Intersection(vec3 rayOrigin, vec3 rayDirection, float& u, float& v, float& w, float* t)
{
	vec3 L = position - rayOrigin;
	float tca = dot(L, rayDirection);
	if (tca < 0)
	{
		return false;
	}
	float s2 = (dot(L, L) - (tca * tca));
	float s = sqrt(s2);
	if (s > radius)
	{
		return false;
	}		
	float thc = sqrt((radius * radius) - s2);
	*t = tca - thc;
			
	return true;
}

vec3 Sphere::CalculateNormal(vec3 arg_ivan, float& u, float& v, float& w, int* Shininess, vec3* DiffuseC, vec3* SpecularC)
{
	*Shininess = 100;
	*DiffuseC = colour * vec3(0.5);
	*SpecularC = vec3(0.7, 0.7, 0.7);
	return (arg_ivan - position);
}




Sphere::~Sphere()
{
}


