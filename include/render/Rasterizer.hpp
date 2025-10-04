#ifndef RASTERIZER_HPP
#define RASTERIZER_HPP

#include <cstdint>
#include <limits.h>
#include <CL/cl.h>
#include "../../include/math/Triangle.hpp"
#include "../../include/core/Camera.hpp"

void DrawTriangleDepthBufferPerformance(
    triangle& triProjected,
    float* depthBuffer,
    uint32_t* frameBuffer,
    int& ScreenSizeX,
    int& ScreenSizeY,
    int& numPixels,
    Camera& camera,
    unsigned int DEPTH_BUFFER_RES_X,
    unsigned int DEPTH_BUFFER_RES_Y);

/*void ShadowMap(
    triangle& tri,
    float* shadowMap,
    int& ScreenSizeX,
    int& ScreenSizeY,
    unsigned int DEPTH_BUFFER_RES_X,
    unsigned int DEPTH_BUFFER_RES_Y,
    int& numPixels,
    int& numShadow);*/

#endif
