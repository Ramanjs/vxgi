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
  bool regenShadowMap;
  bool visualize;
  bool hasShadows;
  bool diffuseGI;
  bool specularGI;
  float floorSpecular;
  glm::vec3 &floorSpecularRef;
  float floorEmissive;
  glm::vec3 &floorEmissiveRef;
  float curtainSpecular;
  glm::vec3 &curtainSpecularRef;
  float curtainEmissive;
  glm::vec3 &curtainEmissiveRef;
  float dynamicSpecular;
  glm::vec3 &dynamicSpecularRef;
  float dynamicEmissive;
  glm::vec3 &dynamicEmissiveRef;
  glm::vec3 lightPosition;
  glm::vec3 lightColor;

public:
  Editor(Scene &_scene, ShadowMap &_shadowMap, VoxelMap &_voxelmap,
         Camera &_camera)
      : engineMode(EngineMode::RENDER), shadowMap(_shadowMap), scene(_scene),
        voxelmap(_voxelmap), camera(_camera),
        floorSpecularRef(_scene.getFloorSpecularRef()),
        floorEmissiveRef(_scene.getFloorEmissiveRef()),
        curtainSpecularRef(_scene.getCurtainSpecularRef()),
        curtainEmissiveRef(_scene.getCurtainEmissiveRef()),
        dynamicSpecularRef(_scene.getDynamicSpecularRef()),
        dynamicEmissiveRef(_scene.getDynamicEmissiveRef()) {
    lightPosition = glm::vec3(200.0f, 2000.0f, 450.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    viewMode = true, voxelRes = 0, hasShadows = true, diffuseGI = true,
    specularGI = true, revoxelize = true, regenShadowMap = true;
  }

  void processCameraInput(GLFWwindow *window);
  void renderEditor();
  void renderScene();
};

#endif /* ifndef EDITOR_H */
