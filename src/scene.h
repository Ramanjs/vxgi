#ifndef SCENE_H
#define SCENE_H

#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "tinyobjloader.h"
#include "utils.h"

#include <map>
#include <vector>

class Scene {
private:
  std::vector<Mesh> meshes;
  std::map<std::string, GLuint> textures;
  std::vector<Material> materials;
  float gMinX, gMinY, gMinZ, gMaxX, gMaxY, gMaxZ;

  void loadTextureFromFile(const char *texturePath, const char *textureName);

public:
  Scene()
      : gMinX(FLT_MAX), gMinY(FLT_MAX), gMinZ(FLT_MAX), gMaxX(FLT_MIN),
        gMaxY(FLT_MIN), gMaxZ(FLT_MIN){};
  void loadObj(const char *textureDir, const char *filePath);
  void draw(Shader &shader, int textureUnit);
  glm::vec3 getWorldCenter();
  float getWorldSize();
  std::vector<glm::vec3> getAABB();
};

#endif /* ifndef SCENE_H */
