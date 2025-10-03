#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <cstdint>
#include<string>
#include<vector>
#include <unordered_map>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <CL/cl.h>

// Local libraries
#include "../include/core/vec4.hpp"
#include "../include/core/mat4.hpp"
#include "../include/core/Camera.hpp"
#include "../include/core/World.hpp"
#include "../include/utils/utils.hpp"
#include "../include/render/Rasterizer.hpp"
#include "../include/math/bk_math.hpp"

#define PI 3.1415926535f

using namespace math;

int main()
{
  SDL_Window* win = NULL;
  SDL_Renderer* renderer = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return 1;

  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  int ScreenSizeX = DM.w - 100;
  int ScreenSizeY = DM.h - 100;
  std::string path = getCurrentDirectory();

  win = SDL_CreateWindow("Game Engine 3D", 100, 100, ScreenSizeX, ScreenSizeY, SDL_WINDOW_SHOWN); // | SDL_WINDOW_FULLSCREEN_DESKTOP);
  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); //  | SDL_RENDERER_PRESENTVSYNC

  // Setup the world
  std::vector<std::string> meshPath;
  std::vector<vec4> meshTranslation;
  std::vector<vec4> meshRotation;

  /*meshPath.push_back(path + "/obj/mountains.obj");
  meshTranslation.push_back(vec4({0.0f, -20.0f, 0.0f, 1.0f}));
  meshRotation.push_back(vec4({0.0f, 0.0f, 0.0f, 0.0f}));*/
  
  meshPath.push_back(path + "/obj/VideoShip.obj");
  meshTranslation.push_back(vec4({0.0f, 0.0f, 10.0f, 1.0f}));
  meshRotation.push_back(vec4({0.0f, 0.0f, 0.0f, 0.0f}));
  
  meshPath.push_back(path + "/obj/VideoShip.obj");
  meshTranslation.push_back(vec4({0.0f, 2.0f, 10.0f, 0.2f}));
  meshRotation.push_back(vec4({0.0f, 0.0f, 0.0f, 0.0f}));

  /*meshPath.push_back(path + "/obj/Wall.obj");
  meshTranslation.push_back(vec4({0.0f, 0.0f, 20.0f, 0.02f}));
  meshRotation.push_back(vec4({PI / 2.0f, 0.0f, 0.0f, 0.0f}));*/

  World world(meshPath, meshTranslation, meshRotation, vec4(0.0f * (PI / 180.0f))); 
  
  float scaleFactor = 100.0f / 100.0f;
  unsigned int BUFFER_RESOLUTIONX = static_cast<unsigned int>(ScreenSizeX * scaleFactor);
  unsigned int BUFFER_RESOLUTIONY = static_cast<unsigned int>(ScreenSizeY * scaleFactor);

  SDL_Texture* frameBuffer_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING,
    BUFFER_RESOLUTIONX,
    BUFFER_RESOLUTIONY
  );

  float* depthBuffer = (float*)malloc(BUFFER_RESOLUTIONX * BUFFER_RESOLUTIONY * sizeof(float));
  uint32_t* frameBuffer = (uint32_t*)malloc(BUFFER_RESOLUTIONX * BUFFER_RESOLUTIONY * sizeof(uint32_t));


  if (!depthBuffer) {
    fprintf(stderr, "Failed to allocate depth buffer\n");
    exit(EXIT_FAILURE);
  }

  // Define the direction vectors
  vec4 UpDirection(0.0, 1.0, 0.0);  // Y-axis, up direction
  vec4 ForwardDirection(0.0, 0.0, 1.0); // Z-axis, forward direction

  Camera camera(vec4(0.0, -3.0, 0.0), 
                ForwardDirection,
                UpDirection, 
                90.0f, 
                90.0f,
                0.1f, 
                1000.0f,
                (static_cast<float>(ScreenSizeX)/static_cast<float>(ScreenSizeY)));

  float Velocity = 0.4;
  Uint32 startTime = SDL_GetTicks();
  int frameCount = 0;
  float fps = 0.0f;
  
  // Track key states
  std::unordered_map<SDL_Keycode, bool> keyStates = {
    {SDLK_UP, false},
    {SDLK_DOWN, false},
    {SDLK_LEFT, false},
    {SDLK_RIGHT, false},
    {SDLK_z, false},
    {SDLK_s, false}
  };

  // SDL_SetRelativeMouseMode(SDL_TRUE); // Capture mouse movement

  bool running = true;
  while (running)
  {
    int numVerts = 0;
    int numPixels = 0;
    int numShadow = 0;

    SDL_SetRenderDrawColor(renderer, 20, 40, 100, 255);
    SDL_RenderClear(renderer);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      } else if (event.type == SDL_KEYDOWN) {
        keyStates[event.key.keysym.sym] = true; // Set key state to true on key down
      } else if (event.type == SDL_KEYUP) {
        keyStates[event.key.keysym.sym] = false; // Set key state to false on key up
      }
    }
    
    // Use the keyStates map to control camera movement
    if (keyStates[SDLK_UP]) {
      camera.moveUp(Velocity);
    }
    if (keyStates[SDLK_DOWN]) {
      camera.moveUp(-Velocity);
    }
    if (keyStates[SDLK_LEFT]) {
      camera.moveRight(-Velocity);
    }
    if (keyStates[SDLK_RIGHT]) {
      camera.moveRight(Velocity);
    }
    if (keyStates[SDLK_z]) {
      camera.moveForward(Velocity);
    }
    if (keyStates[SDLK_s]) {
      camera.moveForward(-Velocity);
    }


    mat4 WorldRotationX, WorldRotationY, WorldRotationZ;

    // Create all the matrix for the world transformation
    WorldRotationX.RotationX(world.WorldRotation.x); // (rotation around X-axis)
    WorldRotationY.RotationY(world.WorldRotation.y); // (rotation around Y-axis)
    WorldRotationZ.RotationZ(world.WorldRotation.z); // (rotation around Z-axis)
    
    // All the transformation of the world (apply to all meshes)
    mat4 WorldTransformation;
    WorldTransformation = WorldRotationZ * WorldRotationX * WorldRotationY;

    // Initialize memory
    for (size_t i = 0; i < (size_t)(BUFFER_RESOLUTIONX * BUFFER_RESOLUTIONY); i++) {
      depthBuffer[i] =  std::numeric_limits<float>::infinity(); // Set each element to infinity
      frameBuffer[i] =  (uint32_t)0xAAAAFFFF;
    }

    std::vector<triangle> projectedTriangles;

    for (auto& mesh : world.meshes) {
      mat4 MeshRotationX, MeshRotationY, MeshRotationZ, MeshTranslation;

      // Mesh transformation (different for each mesh)
      MeshRotationX.RotationX(mesh.rotation.x); // Rotation around X-axis
      MeshRotationY.RotationY(mesh.rotation.y); // Rotation around Y-axis
      MeshRotationZ.RotationZ(mesh.rotation.z); // Rotation around Z-axis

      // Combine rotations (order matters: first Z, then X, then Y)
      mat4 MeshRotation;
      MeshRotation = MeshRotationZ * MeshRotationX * MeshRotationY; // Combining rotations

      // Mesh translation (moving the mesh to its world position)
      MeshTranslation.Translation(mesh.position);

      // Final transformation matrix
      mat4 MeshTransformation;

      // Apply rotation first, then translation
      MeshTransformation = MeshTranslation * MeshRotation;

      // =========================
      // Begin Triangles Processing
      // =========================

      for (auto& tri : mesh.tris)
      {
        triangle triProjected, triViewed, triTransformed, triMeshTransformed, triLightSpace;

        // Apply mesh transformation on each triangle
        for (int o = 0; o < 3; o++) {
          triMeshTransformed.p[o] = MeshTransformation * tri.p[o];
        }
        
        // Apply world transformation on each triangle
        for (int o = 0; o < 3; o++) {  
          triTransformed.p[o] = WorldTransformation * triMeshTransformed.p[o];
        }

        vec4 line1 = triTransformed.p[1] - triTransformed.p[0];
        vec4 line2 = triTransformed.p[2] - triTransformed.p[0];
        
        vec4 normal = line1.crossProduct(line2);
        normal.normalize();

        vec4 vCameraRay = triTransformed.p[0] - camera.position;
        
        if (normal.dot(vCameraRay) < 0.0f) {
          // =================================================================
          // Clip Triangles against the edge of the screen (frustrum clipping)
          // =================================================================

          int nClippedTriangles = 0;
          triangle triClipped[2];
          
          nClippedTriangles = bk_math::Triangle_ClipAgainstPlane(
            vec4(0.0f, 0.0f, 0.1f, 1.0f),
            vec4(0.0f, 0.0f, 1.0f, 1.0f), 
            triTransformed, 
            triClipped[0], 
            triClipped[1]
          );
          
          for (int n = 0; n < nClippedTriangles; n++)
          {                                                                                                  
            for (int o = 0; o < 3; o++) { 
              // ======================================================
              // Convert World Space --> View Space (view matrix) 
              // Project triangles from 3D --> 2D (projection matrix)
              // ======================================================
              triProjected.p[o] = camera.computeSpaceMatrix() * triClipped[n].p[o];
              triProjected.p[o] = triProjected.p[o] / triProjected.p[o].w; 

              // ======================
              // NDC --> Screen Space
              // ======================
              triProjected.p[o].x = (triProjected.p[o].x + 1.0f) * 0.5f * ScreenSizeX;
              triProjected.p[o].y = (triProjected.p[o].y + 1.0f) * 0.5f * ScreenSizeY;
            }

            DrawTriangleDepthBufferPerformance(triProjected, depthBuffer, frameBuffer, ScreenSizeX, ScreenSizeY, numPixels, camera, BUFFER_RESOLUTIONX, BUFFER_RESOLUTIONY);
            numVerts += 1;
          }         
        }
      }
    }
    
    // Update frameBuffer_texture with matrix frameBuffer and Render it on the screen
    SDL_UpdateTexture(frameBuffer_texture, nullptr, frameBuffer, ScreenSizeX * sizeof(uint32_t));
    SDL_RenderCopy(renderer, frameBuffer_texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    // FPS calculation
    frameCount++;
    Uint32 frameEnd = SDL_GetTicks();
    Uint32 elapsedTime = frameEnd - startTime;
    if (elapsedTime > 1000) { // Update every second     
      fps = frameCount / (elapsedTime / 1000.0f);
      frameCount = 0; startTime = frameEnd;

      // Info in the title of the window
      std::string title = "GameEngineInfo : {FPS: " + std::to_string(fps) + \
                          ", Number of Triangles: " + std::to_string(numVerts) + \
                          ", Number of Pixels : " + std::to_string(numPixels) +   \
                          ", Number of Pixels in shadow : " + std::to_string(numShadow) + "}";

      SDL_SetWindowTitle(win, title.c_str());
    }
  }
  // Free allocation in the memory
  free(depthBuffer);
  free(frameBuffer);

  // Clean up SDL allocation
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
};

