#ifndef BK_MATH_HPP
#define BK_MATH_HPP

#include <cmath>
#include "../../include/math/vec4.hpp"
#include "../../include/math/Triangle.hpp"

namespace bk_math {
    double degreesToRadians(double degrees);
    math::vec4 normalize(const math::vec4& v);
    math::vec4 Vector_IntersectPlane(math::vec4& plane_p, math::vec4& plane_n,
                               math::vec4& lineStart, math::vec4& lineEnd, float& t);
    int Triangle_ClipAgainstPlane(math::vec4 plane_p, math::vec4 plane_n,
                                  triangle& in_tri, triangle& out_tri1, triangle& out_tri2);
}

#endif
