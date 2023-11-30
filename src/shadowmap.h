#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "scene.h"
#include "shader.h"
#include "utils.h"

class ShadowMap {
private:
  const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
  GLuint depthMapFBO;
  GLuint depthMapTexure;
  glm::mat4 lightProjection;
  Shader depthMapShader;
  Shader depthMapDebugShader;
  glm::mat4 lightSpaceMatrix;
  GLfloat AABB[6];
  GLuint quadVAO;

  void initTexture();
  void initShader(Scene &scene, glm::vec3 lightPosition);
  void computeLightSpaceAABB(Scene &scene, glm::mat4 &lightView);
  void renderQuad();

public:
  ShadowMap(const char *vsPath, const char *fsPath, const char *debugVsPath,
            const char *debugFsPath)
      : depthMapShader(vsPath, fsPath),
        depthMapDebugShader(debugVsPath, debugFsPath), quadVAO(0) {
    initTexture();
  }

  void generate(Scene &scene, glm::vec3 lightPosition);
  void render();
  glm::mat4 &getLightSpaceMatrix() { return lightSpaceMatrix; }
  GLuint getDepthMapTexture() { return depthMapTexure; }
};

#endif /* ifndef SHADOW_MAP_H */
