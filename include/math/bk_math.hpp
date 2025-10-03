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
    math::vec4 normalize(const math::vec4& v);

    // Intersection entre une ligne (lineStart → lineEnd) et un plan
    math::vec4 Vector_IntersectPlane(math::vec4& plane_p, math::vec4& plane_n,
                               math::vec4& lineStart, math::vec4& lineEnd, float& t);

    // Découpe un triangle par rapport à un plan
    int Triangle_ClipAgainstPlane(math::vec4 plane_p, math::vec4 plane_n,
                                  triangle& in_tri, triangle& out_tri1, triangle& out_tri2);
}

#endif
