#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <string>
#include <SDL2/SDL.h>
#include "../core/vec4.hpp"
#include "../core/Triangle.hpp"
#include "../core/Camera.hpp"

std::string getCurrentDirectory();
int maxValue(int, int);
int minValue(int, int);
double degreesToRadians(double);
double timeToRadians(double&);
vec4 Vector_IntersectPlane(vec4&, vec4&, vec4&, vec4&, float&);
void DrawTriangleDepthBuffer(SDL_Renderer*, triangle&, float*, int, int, int&);
int Triangle_ClipAgainstPlane(vec4, vec4, triangle&, triangle&, triangle&);
std::string loadKernel(const char* filename);

#endif
