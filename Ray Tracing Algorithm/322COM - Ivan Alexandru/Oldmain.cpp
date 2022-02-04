/*
#include <iostream>
#include <algorithm>
#include <fstream>   // std::ofstream
#include <glm/glm.hpp>   
#include <math.h>
#include <vector>


#include "Sphere.h";



using namespace std;
using namespace glm;


//#define PI 3.14159265

float height = 640;
float width = 840;



int main()
{

	Sphere sphere_list[5];
	sphere_list[0] = Sphere(vec3(0, 0, -20), 4, vec3(1.00, 0.32, 0.36)); //red
	sphere_list[1] = Sphere(vec3(5, -1, -15), 2, vec3(0.9, 0.75, 0.4)); //yellow
	sphere_list[2] = Sphere(vec3(5, 0, -25), 4, vec3(0.7, 0.8, 1)); //blue
	sphere_list[3] = Sphere(vec3(-6, 0, -15), 3, vec3(0.8, 0.8, 0.8)); //light gray
	sphere_list[4] = Sphere(vec3(0, -10004, -20), 10000, vec3(0.2, 0.2, 0.2)); // grey floor


	vec3** image = new vec3 * [width];
	for (int i = 0; i < width; i++)
	{
		image[i] = new vec3[height];
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			float pixNormX = (i + 0.5) / width;
			float pixNormY = (j + 0.5) / height;
			float imageAspectRatio = width / height;
			float pixRemapX = (2 * pixNormX - 1) * imageAspectRatio;
			float pixRemapY = 1 - 2 * pixNormY;
			float pixCameraX = pixRemapX * tan(radians(30.0) / 2);
			float pixCameraY = pixRemapY * tan(radians(30.0) / 2);
			vec3 PcameraSpace = vec3(pixCameraX, pixCameraY, -1);
			vec3 rayOrigin = vec3(0, 0, 0);
			vec3 rayDirection = PcameraSpace - rayOrigin;
			rayDirection = normalize(rayDirection);

			float minT = INFINITY;
			int sphereHit = -1;
			float t0 = 0.0f;


			for (int k = 0; k < sizeof(sphere_list) / sizeof(sphere_list[0]); k++)
			{
				bool hit = sphere_list[k].Intersection(&t0, rayOrigin, rayDirection);

				if (hit && t0 < minT)
				{
					minT = t0;
					sphereHit = k;
				}
				if (sphereHit != -1)
				{
					image[i][j] = sphere_list[sphereHit].colour;
				}
				else
				{
					image[i][j] = vec3(1);
				}
			}
		}
	}




	// Save result to a PPM image
	ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			ofs << (unsigned char)(std::min((float)1, (float)image[x][y].x) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].y) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].z) * 255);
		}
	}

	cout << "image created";


	ofs.close();
	return 0;
}*/