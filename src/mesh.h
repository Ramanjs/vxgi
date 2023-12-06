#ifndef MESH_H
#define MESH_H

#include "utils.h"

class Mesh {
public:
  GLuint vao, vbo;
  int numTriangles;
  size_t materialId;
  int isDynamic;
};

#endif /* ifndef MESH_H */
