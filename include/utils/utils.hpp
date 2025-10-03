#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <string>
#include <SDL2/SDL.h>

std::string getCurrentDirectory();
double degreesToRadians(double);
std::string loadKernel(const char* filename);

#endif
