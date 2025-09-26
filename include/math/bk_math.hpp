#ifndef BK_MATH_HPP
#define BK_MATH_HPP

#include <cmath>
#include "../../include/core/vec4.hpp"
#include "../../include/core/Triangle.hpp"

// Namespace pour les fonctions mathématiques utilitaires
namespace bk_math {

    // Convertit des degrés en radians
    double degreesToRadians(double degrees);

    // Normalise un vecteur (retourne le même si norme = 0)
    vec4 normalize(const vec4& v);

    // Intersection entre une ligne (lineStart → lineEnd) et un plan
    vec4 Vector_IntersectPlane(vec4& plane_p, vec4& plane_n,
                               vec4& lineStart, vec4& lineEnd, float& t);

    // Découpe un triangle par rapport à un plan
    int Triangle_ClipAgainstPlane(vec4 plane_p, vec4 plane_n,
                                  triangle& in_tri, triangle& out_tri1, triangle& out_tri2);
}

#endif
