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
  glm::vec3 lightPosition;
  bool viewMode;
  bool revoxelize;
  EngineMode engineMode;
  Scene &scene;
  ShadowMap &shadowMap;
  VoxelMap &voxelmap;
  Camera &camera;

public:
  Editor(Scene &_scene, ShadowMap &_shadowMap, VoxelMap &_voxelmap,
         Camera &_camera)
      : lightPosition(200.0f, 2000.0f, 450.0f), revoxelize(true),
        engineMode(EngineMode::RENDER), shadowMap(_shadowMap), scene(_scene),
        voxelmap(_voxelmap), camera(_camera), viewMode(true) {}

  void processCameraInput(GLFWwindow *window);
  void renderEditor();
  void renderScene();
};

#endif /* ifndef EDITOR_H */
