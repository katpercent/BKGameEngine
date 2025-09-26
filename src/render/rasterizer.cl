__kernel void RasterizeTriangle(
    __global float* depthBuffer,
    __global uint* frameBuffer,
    const float2 v0,
    const float2 v1,
    const float2 v2,
    const float z0,
    const float z1,
    const float z2,
    const int width,
    const int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height) return;

    float px = (float)x + 0.5f;
    float py = (float)y + 0.5f;

    float denom = (v1.y - v2.y) * (v0.x - v2.x) +
                  (v2.x - v1.x) * (v0.y - v2.y);
    float alpha = ((v1.y - v2.y) * (px - v2.x) +
                   (v2.x - v1.x) * (py - v2.y)) / denom;
    float beta  = ((v2.y - v0.y) * (px - v2.x) +
                   (v0.x - v2.x) * (py - v2.y)) / denom;
    float gamma = 1.0f - alpha - beta;

    if (alpha >= 0 && beta >= 0 && gamma >= 0) {
        float z = alpha * z0 + beta * z1 + gamma * z2;
        int index = y * width + x;

        if (z < depthBuffer[index]) {
            depthBuffer[index] = z;
            frameBuffer[index] = 0xFF0000FF; // rouge
        }
    }
}

