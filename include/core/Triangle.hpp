#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "vec4.hpp"

class triangle {
public:
  math::vec4 p[3];
  math::vec4 col[3];
  //vec2 tex[3];
  math::vec4 normal;
};

#endif
