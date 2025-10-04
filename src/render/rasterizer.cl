// ===================================================
// GPU Rasterizer Kernel (OpenCL)
// Equivalent of DrawTriangleDepthBufferPerformance()
// ===================================================

typedef struct {
    float4 p[3];   // triangle vertices
    float4 col[3]; // optional colors
    float4 normal;
} triangle;

// Kernel: draws one triangle to depth + frame buffers
__kernel void draw_triangle_depthbuffer(
    __global float* depthBuffer,
    __global uint* frameBuffer,
    __global const triangle* triangles,
    const int numTriangles,
    const int ScreenSizeX,
    const int ScreenSizeY,
    const int DEPTH_BUFFER_RES_X,
    const int DEPTH_BUFFER_RES_Y)
{
    int tid = get_global_id(0);
    if (tid >= numTriangles)
        return;

    // Load triangle data from global memory
    triangle tri = triangles[tid];

    // Compute scale factors
    const float scaleX = (float)DEPTH_BUFFER_RES_X / (float)ScreenSizeX;
    const float scaleY = (float)DEPTH_BUFFER_RES_Y / (float)ScreenSizeY;

    // Find bounding box in depth buffer coordinates
    int minX = max(0, (int)(fmin(tri.p[0].x, fmin(tri.p[1].x, tri.p[2].x)) * scaleX));
    int maxX = min(DEPTH_BUFFER_RES_X - 1, (int)(fmax(tri.p[0].x, fmax(tri.p[1].x, tri.p[2].x)) * scaleX));
    int minY = max(0, (int)(fmin(tri.p[0].y, fmin(tri.p[1].y, tri.p[2].y)) * scaleY));
    int maxY = min(DEPTH_BUFFER_RES_Y - 1, (int)(fmax(tri.p[0].y, fmax(tri.p[1].y, tri.p[2].y)) * scaleY));

    // Convert back to screen-space for barycentric interpolation
    float startScreenX = (float)minX / scaleX;
    float startScreenY = (float)minY / scaleY;

    // Pixel step
    const float pixelStepX = ceil(1.0f / scaleX);
    const float pixelStepY = ceil(1.0f / scaleY);

    // Precompute barycentric denominator
    float denom = (tri.p[1].y - tri.p[2].y) * (tri.p[0].x - tri.p[2].x)
                + (tri.p[2].x - tri.p[1].x) * (tri.p[0].y - tri.p[2].y);
    if (fabs(denom) < 1e-6f) return;
    float invDenom = 1.0f / denom;

    // Iterate through pixels
    for (int y = minY; y <= maxY; ++y)
    {
        float screenY = startScreenY + (y - minY) * pixelStepY;
        for (int x = minX; x <= maxX; ++x)
        {
            float screenX = startScreenX + (x - minX) * pixelStepX;

            // Barycentric coordinates
            float alpha = ((tri.p[1].y - tri.p[2].y) * (screenX - tri.p[2].x)
                        + (tri.p[2].x - tri.p[1].x) * (screenY - tri.p[2].y)) * invDenom;

            float beta = ((tri.p[2].y - tri.p[0].y) * (screenX - tri.p[2].x)
                       + (tri.p[0].x - tri.p[2].x) * (screenY - tri.p[2].y)) * invDenom;

            float gamma = 1.0f - alpha - beta;

            // Check if pixel is inside the triangle
            if (alpha >= 0 && beta >= 0 && gamma >= 0)
            {
                // Interpolate depth
                float pixelDepth = alpha * tri.p[0].z + beta * tri.p[1].z + gamma * tri.p[2].z;
                int index = y * DEPTH_BUFFER_RES_X + x;

                if (index < 0 || index >= DEPTH_BUFFER_RES_X * DEPTH_BUFFER_RES_Y)
                    continue;

                // Depth test (atomic compare recommended for real GPU safety)
                if (pixelDepth < depthBuffer[index])
                {
                    depthBuffer[index] = pixelDepth;

                    // Simple color (white)
                    frameBuffer[y * ScreenSizeX + x] = 0xFFFFFFFF;
                }
            }
        }
    }
}

