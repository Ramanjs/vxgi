#ifndef SCENE_H
#define SCENE_H

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
  std::vector<tinyobj::material_t> materials;

public:
  Scene(){};
  void loadObj(const char *textureDir, const char *filePath);
  void draw(Shader &shader);
};

#endif /* ifndef SCENE_H */
