#include "shadowmap.h"
#include "constants.h"

void ShadowMap::initTexture() {
  glGenFramebuffers(1, &depthMapFBO);
  glGenTextures(1, &depthMapTexure);
  glBindTexture(GL_TEXTURE_2D, depthMapTexure);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMapTexure, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::initShader() {
  depthMapShader.use();
  glm::mat4 modelT(1.0f);
  depthMapShader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  glm::mat4 lightView =
      glm::lookAt(glm::vec3(lightPosition), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  lightSpaceMatrix = lightProjection * lightView;
  depthMapShader.setUniform(uniformType::mat4x4,
                            glm::value_ptr(lightSpaceMatrix),
                            "lightSpaceMatrix");
}

void ShadowMap::generate(Scene &scene) {
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  initShader();
  scene.draw(depthMapShader, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // reset viewport
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
