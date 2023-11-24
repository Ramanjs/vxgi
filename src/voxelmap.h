#ifndef VOXEL_MAP_H
#define VOXEL_MAP_H

#include "camera.h"
#include "scene.h"
#include "shader.h"
#include "shadowmap.h"
#include "utils.h"

#include <vector>

class VoxelMap {
private:
  GLuint voxelTexture;
  Shader voxelizeShader;
  Shader visualizationShader;
  Shader renderShader;
  Scene &scene;
  ShadowMap &shadowMap;

public:
  VoxelMap(const char *voxelizeVsPath, const char *voxelizeFsPath,
           const char *voxelizeGsPath, const char *visualizeVsPath,
           const char *visualizeFsPath, const char *renderVsPath,
           const char *renderFsPath, Scene &_scene, ShadowMap &_shadowMap)
      : scene(_scene), shadowMap(_shadowMap),
        voxelizeShader(voxelizeVsPath, voxelizeFsPath, voxelizeGsPath),
        visualizationShader(visualizeVsPath, visualizeFsPath),
        renderShader(renderVsPath, renderFsPath) {
    initTexture();
  }

  void initVoxelizeShader(const char *vsPath, const char *fsPath,
                          const char *gsPath);
  void initVisualizationShader(const char *vsPath, const char *fsPath);
  void initRenderShader(const char *vsPath, const char *fsPath);

  void voxelize(glm::vec3 lightPosition);
  void visualize(Camera &camera);
  void render(Camera &camera, glm::vec3 lightPosition);

private:
  void initTexture();
  void clear();
};

#endif /* ifndef VOXEL_MAP_H */
