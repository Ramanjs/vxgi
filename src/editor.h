#ifndef EDITOR_H
#define EDITOR_H

#include "camera.h"
#include "constants.h"
#include "scene.h"
#include "shadowmap.h"
#include "utils.h"
#include "voxelmap.h"

enum class EngineMode { VISUALIZE, RENDER };

class Editor {
private:
  EngineMode engineMode;
  Scene &scene;
  ShadowMap &shadowMap;
  VoxelMap &voxelmap;
  Camera &camera;

  int voxelRes;
  bool viewMode;
  bool revoxelize;
  bool visualize;
  bool shadows;
  bool diffuseGI;
  bool specularGI;
  float floorSpecular;
  float floorEmissive;
  float curtainSpecular;
  float curtainEmissive;
  float dragonSpecular;
  float dragonEmissive;
  glm::vec3 lightPosition;
  glm::vec3 lightColor;
  glm::vec3 dragonPosition;

public:
  Editor(Scene &_scene, ShadowMap &_shadowMap, VoxelMap &_voxelmap,
         Camera &_camera)
      : engineMode(EngineMode::RENDER), shadowMap(_shadowMap), scene(_scene),
        voxelmap(_voxelmap), camera(_camera) {
    lightPosition = glm::vec3(200.0f, 2000.0f, 450.0f);
    viewMode = true, voxelRes = 0, shadows = true, diffuseGI = true,
    specularGI = true, revoxelize = true;
  }

  void processCameraInput(GLFWwindow *window);
  void renderEditor();
  void renderScene();
};

#endif /* ifndef EDITOR_H */
