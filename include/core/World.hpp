#ifndef MESHES_HPP
#define MESHES_HPP

#include "Mesh.hpp"
#include "vec4.hpp"

class World {
public:
	std::vector<Mesh> meshes;
	std::string PathObject3D;
  std::vector<std::string> mesh;
  std::vector<math::vec4> MeshTranslation;
  std::vector<math::vec4> MeshRotationSize;
  math::vec4 WorldRotation;
  World(std::vector<std::string>, std::vector<math::vec4>, std::vector<math::vec4>, math::vec4);
};

#endif
