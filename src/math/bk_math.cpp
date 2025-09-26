#include "../../include/math/bk_math.hpp"

double bk_math::degreesToRadians(double degrees) {
    return degrees * (M_PI / 180.0);
}

vec4 bk_math::normalize(const vec4& v) {
    if (v.magnitude() == 0) {
        return v; // évite la division par 0
    }
    return v / v.magnitude();
}

// Computes the intersection point between a line segment and a plane
vec4 bk_math::Vector_IntersectPlane(vec4& plane_p, vec4& plane_n,
                                    vec4& lineStart, vec4& lineEnd, float& t) 
{
    plane_n.normalize();  // Ensure the plane normal is normalized

    // Plane equation: n·x + d = 0
    float plane_d = -(plane_n.dot(plane_p));

    // Distances from line endpoints to the plane
    float ad = lineStart.dot(plane_n);
    float bd = lineEnd.dot(plane_n);

    // Compute the interpolation factor t where the line intersects the plane
    t = (-plane_d - ad) / (bd - ad);

    // Direction vector of the line
    vec4 lineStartToEnd = lineEnd - lineStart;

    // Scale the direction vector by t to reach the intersection
    vec4 lineToIntersect = lineStartToEnd * t;

    // Return the intersection point
    return lineStart + lineToIntersect;
}


// Clips a triangle against a plane and returns the resulting triangle(s)
int bk_math::Triangle_ClipAgainstPlane(vec4 plane_p, vec4 plane_n,
                                       triangle& in_tri, triangle& out_tri1, triangle& out_tri2) 
{
    plane_n.normalize();  // Ensure the plane normal is normalized

    // Lambda to compute signed distance from a point to the plane
    auto dist = [&](vec4& p) {
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
    };

    // Arrays to hold vertices classified as inside or outside
    vec4* inside_points[3];  int nInsidePointCount = 0;
    vec4* outside_points[3]; int nOutsidePointCount = 0;

    // Compute distances of each vertex of the triangle to the plane
    float d0 = dist(in_tri.p[0]);
    float d1 = dist(in_tri.p[1]);
    float d2 = dist(in_tri.p[2]);

    // Classify vertices: inside if distance >= 0, else outside
    if (d0 >= 0) inside_points[nInsidePointCount++] = &in_tri.p[0];
    else outside_points[nOutsidePointCount++] = &in_tri.p[0];

    if (d1 >= 0) inside_points[nInsidePointCount++] = &in_tri.p[1];
    else outside_points[nOutsidePointCount++] = &in_tri.p[1];

    if (d2 >= 0) inside_points[nInsidePointCount++] = &in_tri.p[2];
    else outside_points[nOutsidePointCount++] = &in_tri.p[2];

    // Case 1: all points are outside → no valid triangle
    if (nInsidePointCount == 0) {
        return 0;
    }

    // Case 2: all points are inside → original triangle is valid
    if (nInsidePointCount == 3) {
        out_tri1 = in_tri;
        return 1;
    }

    // Case 3: one point inside, two outside → triangle shrinks
    if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
        // Copy colors from original
        for (int i = 0; i < 3; ++i)
            out_tri1.col[i] = in_tri.col[i];

        // Keep the inside point
        out_tri1.p[0] = *inside_points[0];

        float t;
        // Find intersection with each of the two outside edges
        out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);

        return 1;
    }

    // Case 4: two points inside, one outside → clipped triangle becomes a quad
    if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
        // First new triangle
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[1] = *inside_points[1];

        float t;
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);

        // Second new triangle (the remaining part of the quad)
        out_tri2.p[0] = *inside_points[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);

        // Copy colors
        for (int i = 0; i < 3; ++i) {
            out_tri1.col[i] = in_tri.col[i];
            out_tri2.col[i] = in_tri.col[i];
        }

        return 2;
    }

    // Should not reach here
    return -1;
}

