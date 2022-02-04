#include <iostream>
#include <fstream>
#include <algorithm>
#include <glm/glm.hpp>
#include <ppl.h>
#include <SDL.h>
#include <vector>

#include "Shape.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "AAB.h"

using namespace std;
using namespace glm;


void createShapes();
vec3 rayTrace(vec3 p, vec3 v);
void setPixel(SDL_Surface* surface);
bool drawPixel(SDL_Surface* surface, int i, int j, int x, int y, int z);

float height = 640.0f;
float width = 840.0f;

bool hardShadows = true;

float printOnce = 1;
float FOV = 90.0f;
SDL_Event event = { 0 };

vector<Shape*> shapeVector;

//2D ARRAY FO THE SCREEN
vec3** image = new vec3 * [width]; 

//CREATING AREA LIGHT FROM A BOX - AAB CLASS
AAB* areaLight = new AAB(vec3(-4.5f, 20, -4.5f), vec3(9, 0.1f, 9));

//SETS EACH PIXEL TO A COLOUR SDL LIBRARY fUNCTIONS
bool drawPixel(SDL_Surface* surface, int x, int y, int r, int g, int b) {

	if (y < 0 || y >= height || x < 0 || x >= width) {
		return false;
	}

	Uint32 colorSDL = SDL_MapRGB(surface->format, r, g, b);
	Uint32* bufp;

	bufp = (Uint32*)surface->pixels + y * surface->pitch / 4 + x;
	*bufp = colorSDL;
	bufp += surface->pitch / 4;

	return true;
}

void setPixel(SDL_Surface* surface) {

	concurrency::parallel_for(int(0), (int)width, [&](int i) {
		for (int j = 0; j < height; ++j) {
			float pixNormX = (i + 0.5f) / width;
			float pixNormY = (j + 0.5f) / height;
			float imageAspectRatio = width / height;
			float pixRemapX = (2 * pixNormX - 1) * imageAspectRatio;
			float pixRemapY = 1 - 2 * pixNormY;
			float pixCameraX = pixRemapX * tan(radians(FOV) / 2);
			float pixCameraY = pixRemapY * tan(radians(FOV) / 2);
			vec3 PcameraSpace = vec3(pixCameraX, pixCameraY, -1);
			vec3 rayOrigin = vec3(0);
			vec3 rayDirection = normalize(PcameraSpace - rayOrigin);

			vec3 pixelColour = rayTrace(rayOrigin, rayDirection);
			image[i][j] = pixelColour;
			drawPixel(surface, i, j, std::min(1.0f, image[i][j].x) * 255, std::min(1.0f, image[i][j].y) * 255, std::min(1.0f, image[i][j].z) * 255);
		}
		});
}

void createShapes() 
{
	//SPHERES
	shapeVector.push_back(new Sphere(vec3(0, 0, -20), 4, vec3(1, 0.32f, 0.36f)));
	shapeVector.push_back(new Sphere(vec3(5, -1, -15), 2, vec3(0.9f, 0.76f, 0.46f)));
	shapeVector.push_back(new Sphere(vec3(5, 0, -25), 3, vec3(0.65f, 0.77f, 0.97f)));
	shapeVector.push_back(new Sphere(vec3(-5.5f, 0, -15), 3, vec3(0.9f)));

	//PLANE
	shapeVector.push_back(new Plane(vec3(0, -4, 0), normalize(vec3(0, 1, 0)), vec3(0.3f))); //normalize??

	//TRIANGLE
	//shapeVector.push_back(new Triangle(vec3(0), vec3(0), vec3(0), vec3(0), vec3(0), vec3(0), vec3(0.4)));
}

vec3 rayTrace(vec3 RayOrigin, vec3 RayDirection) {

	vec3 pixelColour;
	vec3 uvw, minUVW = vec3(0);

	float t0 = 0.0f;
	float minT = FLT_MAX;
	int shapeHit = -1;

	//LOOKS IN THE SCENE FOR SHAPES
	for (int k = 0; k < shapeVector.size(); ++k) {

		bool hit = shapeVector[k]->Intersection(RayOrigin, RayDirection, uvw.x, uvw.y, uvw.z, &t0);

		if (hit && t0 < minT) {
			minT = t0;
			minUVW.x = uvw.x;
			minUVW.y = uvw.y;
			minUVW.z = uvw.z;
			shapeHit = k;
		}
	}

	if (shapeHit != -1) {

		//ARG_IVAN TAKES THE POSITION OF THE HIT
		vec3 arg_ivan = RayOrigin + (minT * RayDirection);


		//LIGHT PROPERTIES 
		vec3 areaLightPos = vec3(areaLight->position.x, areaLight->position.y, areaLight->position.z);
		vec3 areaLightSize = vec3(areaLight->size.x, areaLight->size.y, areaLight->size.z);
		vec3 areaLightCenter = vec3(areaLightPos.x + (areaLightSize.x / 2), areaLightPos.y + (areaLightSize.y / 2), areaLightPos.z + (areaLightSize.z / 2));
		vec3 lightIntensity = vec3(1);
		vec3 DiffuseC, SpecularC = vec3(0);
		int Shininess = 0;

		//DIFFUSE LIGHTING
		vec3 normal = normalize(shapeVector[shapeHit]->CalculateNormal(arg_ivan, minUVW.x, minUVW.y, minUVW.z, &Shininess, &DiffuseC, &SpecularC));
		vec3 ambient = shapeVector[shapeHit]->colour * vec3(0.07f);
		vec3 lightRay = normalize(areaLightCenter - arg_ivan);
		vec3 diffuse = DiffuseC * lightIntensity * glm::max(0.0f, dot(lightRay, normal));

		//SPECULAR LIGHTING
		vec3 reflection = normalize(2 * (dot(lightRay, normal)) * normal - lightRay);
		float maxCalc = glm::max(0.0f, dot(reflection, normalize(RayOrigin - arg_ivan)));
		vec3 specular = SpecularC * lightIntensity * (float)pow(maxCalc, Shininess);

		int lightShapeHit = -1;


		//HARD SHADOWS
		if (hardShadows) {

			for (int l = 0; l < shapeVector.size(); ++l) {

				bool lightingHit = shapeVector[l]->Intersection(arg_ivan + (1e-4f * normal), lightRay, uvw.x, uvw.y, uvw.z, &t0);

				if (lightingHit && t0 < minT) {
					minT = t0;
					lightShapeHit = l;
				}
			}

			if (lightShapeHit != -1) {

				pixelColour = ambient;
			}
			else {

				pixelColour = (diffuse + specular);
			}
		}

		else {

			//SOFT SHADOWS
			//THE NUMBER OF SAMPLES ARE THE AMOUNT OF RAYS THAT ARE GOING TO BE SQUARED
			//TO CALCULATE THE INCREMENT, SMALLER INCREMENT THE BETTER THE SOFT SHADOW
			//INCREASE THE NUMBER THE NUMBER OF SAMPLES FOR A NEW INCREMENT.

			//IN THE DOCUMENTATION I'VE SHOWN THE COMPARISON BETWEEN 5 SAMPLES AND 9 SAMPLES.
			//TRY 5 SAMPLES.
			float sample = 9.0f;
			float softIncrement = areaLightSize.x / sample;
			float hitRays = 1.0f;
			float totalRays = 0.0f;

			totalRays = pow(sample, 2);

			for (float m = 0; m < areaLightSize.x; m += softIncrement) {

				for (float n = 0; n < areaLightSize.z; n += softIncrement) {

					float t0s = 0.0f;
					float minTs = FLT_MAX;
					lightShapeHit = -1;

					vec3 UVWs = vec3(0);

					areaLightPos = vec3(m, areaLightPos.y, n);
					lightRay = normalize(areaLightPos - arg_ivan);

					for (int l = 0; l < shapeVector.size(); ++l) {

						if (shapeHit != l) {

							bool lightingHit = shapeVector[l]->Intersection(arg_ivan + (1e-4f * normal), lightRay, UVWs.x, UVWs.y, UVWs.z, &t0s);

							if (lightingHit && t0s < minTs) {

								minTs = t0s;
								lightShapeHit = l;
							}
						}
					}

					if (lightShapeHit != -1) {

						hitRays = hitRays - (1 / totalRays);
					}
				}
			}

			pixelColour = vec3((hitRays) * (diffuse + specular));
		}
	}
	else {

		pixelColour = vec3(0.1, 0.2, 0.3);
	}

	return pixelColour;
}

bool done(bool quit_if_esc, bool delay) {

	if (delay) {
		SDL_Delay(5);
	}

	bool done = false;

	if (!SDL_PollEvent(&event)) {
		return 0;
	}

	if (event.type == SDL_QUIT) {
		done = true;
	}

	else if (event.type == SDL_KEYDOWN) {

		switch (event.key.keysym.sym) {

		case SDLK_ESCAPE:
			done = true;
			break;

		default:
			break;
		}
	}

	return done;
}

int main(int argc, char* args[]) {


	for (int i = 0; i < width; ++i) {
		image[i] = new vec3[height];
	}

	SDL_Window* window = NULL;

	SDL_Surface* screenSurface = NULL;

	window = SDL_CreateWindow("Ray Tracing-322COM-Ivan Alexnadru", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	screenSurface = SDL_GetWindowSurface(window);

	while (!done(true, false)) {

		SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

		createShapes();
		setPixel(screenSurface);

		SDL_UpdateWindowSurface(window);
	
		//USER INTERACTION
		if (printOnce == 1) {
			printOnce = 0;
			cout << "Press 'S' to toggle hard/soft shadows\n" << "Use up and down to change the FOV\n" << endl;
		}

		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_UP:
				if (FOV > 20.0f) FOV -= 10.0f;
				cout << "Decreasing the FOV\n";
				break;

			case SDLK_DOWN:
				if (FOV < 150.0f) FOV += 10.0f;
				cout << "Increasing the FOV\n";
				break;

			case SDLK_s:
				hardShadows = !hardShadows;
				cout << "Switching the shadows, please wait...\n";
				break;
				
			default:
				break;
			}
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();


	//STOPPING MEMORY LEAK MEASURES
	delete areaLight;

	for (int i = 0; i < width; ++i) {
		delete[] image[i];
	}
	delete[] image;

	for (int i = 0; i < shapeVector.size(); ++i) {
		delete shapeVector[i];
	}

	return 0;
}