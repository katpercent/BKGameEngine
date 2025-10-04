#include "../../include/render/Rasterizer.hpp"

// Add a triangle on the frame buffer
void DrawTriangleDepthBufferPerformance(
  triangle& triProjected,
  float* depthBuffer,
  uint32_t* frameBuffer,
  int& ScreenSizeX,
  int& ScreenSizeY,
  int& numPixels,
  Camera& camera,
  unsigned int DEPTH_BUFFER_RES_X,
  unsigned int DEPTH_BUFFER_RES_Y)
{
  const float scaleX = static_cast<float>(DEPTH_BUFFER_RES_X) / ScreenSizeX;
  const float scaleY = static_cast<float>(DEPTH_BUFFER_RES_Y) / ScreenSizeY;


  // Find the edges of the triangle (min and max in depth buffer space)
  int minX = std::max(0, static_cast<int>(std::min(triProjected.p[0].x, std::min(triProjected.p[1].x, triProjected.p[2].x)) * scaleX));
  int maxX = std::min(static_cast<int>(DEPTH_BUFFER_RES_X) - 1, static_cast<int>(std::max(triProjected.p[0].x, std::max(triProjected.p[1].x, triProjected.p[2].x)) * scaleX));
  int minY = std::max(0, static_cast<int>(std::min(triProjected.p[0].y, std::min(triProjected.p[1].y, triProjected.p[2].y)) * scaleY));
  int maxY = std::min(static_cast<int>(DEPTH_BUFFER_RES_Y) - 1, static_cast<int>(std::max(triProjected.p[0].y, std::max(triProjected.p[1].y, triProjected.p[2].y)) * scaleY));
  
  float startScreenX = minX / scaleX;
  float startScreenY = minY / scaleY;

  
  const int pixelWidth = static_cast<int>(std::ceil(1.0f / scaleX)); // Block width in screen space
  const int pixelHeight = static_cast<int>(std::ceil(1.0f / scaleY)); // Block height in screen space

  float denom = (triProjected.p[1].y - triProjected.p[2].y) * (triProjected.p[0].x - triProjected.p[2].x) +
              (triProjected.p[2].x - triProjected.p[1].x) * (triProjected.p[0].y - triProjected.p[2].y);
  float invDenom = 1.0f / denom;

  float screenY = startScreenY;
  // Loop over depth buffer pixels
  for (int y = minY; y <= maxY; ++y, screenY += pixelWidth) {
    float screenX = startScreenX;
    for (int x = minX; x <= maxX; ++x, screenX += pixelHeight) {
      // Calculate barycentric coordinates
      float alpha = ((triProjected.p[1].y - triProjected.p[2].y) * (screenX - triProjected.p[2].x) +
                     (triProjected.p[2].x - triProjected.p[1].x) * (screenY - triProjected.p[2].y)) * invDenom;

      float beta = ((triProjected.p[2].y - triProjected.p[0].y) * (screenX - triProjected.p[2].x) +
                    (triProjected.p[0].x - triProjected.p[2].x) * (screenY - triProjected.p[2].y)) * invDenom;

      float gamma = 1.0f - alpha - beta;

      // Check if the pixel is inside the triangle
      if (alpha >= 0 && beta >= 0 && gamma >= 0)
      {
        // Interpolate depth value
        float pixelDepth = alpha * triProjected.p[0].z + beta * triProjected.p[1].z + gamma * triProjected.p[2].z;

        // Calculate depth buffer index
        int index = y * DEPTH_BUFFER_RES_X + x;
  
        // Skip this iteration if the index is out of the screen resolution
        if (index < 0 || index >= DEPTH_BUFFER_RES_X * DEPTH_BUFFER_RES_Y) {
        fprintf(stderr, "Index out of bounds: %d\n", index);
        continue; 
        }


        // Update depth buffer if the pixel is closer
        if (pixelDepth < depthBuffer[index])
        {
          depthBuffer[index] = pixelDepth;
          numPixels++;

          math::vec4 point = triProjected.p[0] * alpha + triProjected.p[1] * beta + triProjected.p[2] * gamma;

          frameBuffer[y * ScreenSizeX + x] = 0xFFFFFFFF;
        }
      }
    }
  }
}

// Depth buffer from the light view (not implemented for now)
/*void ShadowMap(
    triangle& tri,
    float* shadowMap,
    int& ScreenSizeX,
    int& ScreenSizeY,
    unsigned int DEPTH_BUFFER_RES_X,
    unsigned int DEPTH_BUFFER_RES_Y,
    int& numPixels)
{
    

    const float scaleX = static_cast<float>(DEPTH_BUFFER_RES_X) / ScreenSizeX;
    const float scaleY = static_cast<float>(DEPTH_BUFFER_RES_Y) / ScreenSizeY;

    const int pixelWidth = static_cast<int>(std::ceil(1.0f / scaleX)); // Block width in screen space
    const int pixelHeight = static_cast<int>(std::ceil(1.0f / scaleY)); // Block height in screen space

    // Find the edges of the triangle (min and max in depth buffer space)
    int minX = std::max(0, static_cast<int>(std::min(tri.p[0].x, std::min(tri.p[1].x, tri.p[2].x)) * scaleX));
    int maxX = std::min(static_cast<int>(DEPTH_BUFFER_RES_X) - 1, static_cast<int>(std::max(tri.p[0].x, std::max(tri.p[1].x, tri.p[2].x)) * scaleX));
    int minY = std::max(0, static_cast<int>(std::min(tri.p[0].y, std::min(tri.p[1].y, tri.p[2].y)) * scaleY));
    int maxY = std::min(static_cast<int>(DEPTH_BUFFER_RES_Y) - 1, static_cast<int>(std::max(tri.p[0].y, std::max(tri.p[1].y, tri.p[2].y)) * scaleY));
    
    // Loop over depth buffer pixels
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // Map depth buffer pixel to screen space
            float screenX = x / scaleX;
            float screenY = y / scaleY;

            // Calculate barycentric coordinates
            float alpha = ((tri.p[1].y - tri.p[2].y) * (screenX - tri.p[2].x) +
                           (tri.p[2].x - tri.p[1].x) * (screenY - tri.p[2].y)) /
                          ((tri.p[1].y - tri.p[2].y) * (tri.p[0].x - tri.p[2].x) +
                           (tri.p[2].x - tri.p[1].x) * (tri.p[0].y - tri.p[2].y));
            float beta = ((tri.p[2].y - tri.p[0].y) * (screenX - tri.p[2].x) +
                          (tri.p[0].x - tri.p[2].x) * (screenY - tri.p[2].y)) /
                         ((tri.p[1].y - tri.p[2].y) * (tri.p[0].x - tri.p[2].x) +
                          (tri.p[2].x - tri.p[1].x) * (tri.p[0].y - tri.p[2].y));
            float gamma = 1.0f - alpha - beta;

            // Check if the pixel is inside the triangle
            if (alpha >= 0 && beta >= 0 && gamma >= 0)
            {
                // Interpolate depth value
                float pixelDepth = alpha * tri.p[0].z + beta * tri.p[1].z + gamma * tri.p[2].z;

                // Calculate depth buffer index
                int index = y * DEPTH_BUFFER_RES_X + x;

                if (index < 0 || index >= DEPTH_BUFFER_RES_X * DEPTH_BUFFER_RES_Y) {
                fprintf(stderr, "Index out of bounds: %d\n", index);
                continue; // Skip this iteration
                }


                // Update depth buffer if the pixel is closer
                if (pixelDepth < shadowMap[index])
                {
                    numPixels++;
                    shadowMap[index] = pixelDepth;
                }
            }
        }
    }
}*/
