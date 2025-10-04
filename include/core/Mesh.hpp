#ifndef MESH_HPP
#define MESH_HPP

#include "../math/Triangle.hpp"
#include "../math/vec4.hpp"
#include <vector>
#include <string>


class Mesh {
public:
	std::string path;
  std::vector<triangle> tris;
  math::vec4 position;
  math::vec4 rotation;
  bool LoadFromObjectFile(const std::string& sFilename);
	void Scale(float scaleFactor);
	void ChangePos(float x, float y, float z);
};

#endif
