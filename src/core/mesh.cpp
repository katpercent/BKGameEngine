#include "../../include/core/Mesh.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

void Mesh::Scale(float scaleFactor) {
    for (auto& tri : tris) {
        for (auto& vertex : tri.p) {
            vertex.x *= scaleFactor;
            vertex.y *= scaleFactor;
            vertex.z *= scaleFactor;
        } 
    }
}

void Mesh::ChangePos(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}


bool Mesh::LoadFromObjectFile(const std::string& sFilename) {
    // Try to open the OBJ file
    std::ifstream f(sFilename);
    if (!f.is_open()) {
        std::cerr << "Failed to open file: " << sFilename << std::endl;
        return false;
    }

    // Temporary list of vertices read from the file
    std::vector<math::vec4> verts;

    // Read the file line by line
    while (!f.eof()) {
        char line[128];
        f.getline(line, 128);

        std::stringstream s;
        s << line;

        char junk;

        // If the line starts with "v " → it's a vertex definition
        if (line[0] == 'v' && line[1] == ' ') {
            math::vec4 v;
            // Read x, y, z coordinates (OBJ format doesn't usually store w)
            s >> junk >> v.x >> v.y >> v.z;
            verts.push_back(v);
        }
        // If the line starts with "f " → it's a face definition
        else if (line[0] == 'f' && line[1] == ' ') {
            std::vector<int> f;
            std::string segment;

            // Split the face line into vertex indices
            while (s >> segment) {
                if (segment != "f") {
                    std::stringstream segmentStream(segment);
                    std::string indexStr;
                    // Extract only the vertex index (before '/')
                    getline(segmentStream, indexStr, '/');
                    f.push_back(stoi(indexStr) - 1); // OBJ indices start at 1
                }
            }

            // If it's a triangle (3 vertices)
            if (f.size() == 3) {
                tris.push_back({ verts[f[0]], verts[f[1]], verts[f[2]] });
            }
            // If it's a polygon with more than 3 vertices → triangulate it
            else if (f.size() > 3) {
                for (size_t i = 1; i < f.size() - 1; ++i) {
                    tris.push_back({ verts[f[0]], verts[f[i]], verts[f[i + 1]] });
                }
            }
        }
    }
    return true;
}

