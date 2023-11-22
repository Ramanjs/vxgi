#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "scene.h"
#include "shader.h"
#include "utils.h"

class ShadowMap {
private:
  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
  GLuint depthMapFBO;
  GLuint depthMapTexure;
  glm::mat4 lightProjection;
  Shader depthMapShader;
  glm::vec3 &lightPosition;
  glm::mat4 lightSpaceMatrix;

  void initTexture();
  void initShader();

public:
  ShadowMap(const char *vsPath, const char *fsPath, glm::vec3 &_lightPosition)
      : depthMapShader(vsPath, fsPath), lightPosition(_lightPosition) {
    float near_plane = 1.0f, far_plane = 5000.0f;
    lightProjection =
        glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane);
    ;

    initTexture();
    initShader();
  }

  void generate(Scene &scene);
  glm::mat4 &getLightSpaceMatrix() { return lightSpaceMatrix; }
  GLuint getDepthMapTexture() { return depthMapTexure; }
};

#endif /* ifndef SHADOW_MAP_H */
