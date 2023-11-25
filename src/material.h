#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"

class Material {
public:
  glm::vec3 kd;
  glm::vec3 ks;
  GLfloat shininess; // Phong's exponent
  GLuint diffuseMap;
  GLuint specularMap;
  GLuint normalMap;

  Material()
      : kd(0.0f), ks(0.0f), shininess(0.0f), diffuseMap(0), specularMap(0),
        normalMap(0) {}
};

#endif /* ifndef MATERIAL_H */
