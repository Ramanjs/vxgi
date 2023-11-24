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

void ShadowMap::computeLightSpaceAABB(Scene &scene, glm::mat4 &lightView) {
  std::vector<glm::vec3> aabb = scene.getAABB();

  GLfloat minX, maxX, minY, maxY, minZ, maxZ;
  minX = minY = minZ = FLT_MAX;
  maxX = maxY = maxZ = FLT_MIN;
  for (const auto &v : aabb) {
    glm::vec3 lv = lightView * glm::vec4(v, 1.0);

    minX = std::min(minX, lv.x);
    minY = std::min(minY, lv.y);
    minZ = std::min(minZ, lv.z);
    maxX = std::max(maxX, lv.x);
    maxY = std::max(maxY, lv.x);
    maxZ = std::max(maxZ, lv.x);
  }

  AABB[0] = minX;
  AABB[1] = maxX;
  AABB[2] = minY;
  AABB[3] = maxY;
  AABB[4] = minZ;
  AABB[5] = maxZ;
}

void ShadowMap::initShader(Scene &scene) {
  depthMapShader.use();
  glm::mat4 modelT(1.0f);
  depthMapShader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  glm::mat4 lightView =
      glm::lookAt(glm::vec3(lightPosition), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  computeLightSpaceAABB(scene, lightView);
  lightProjection =
      glm::ortho(AABB[0], AABB[1], AABB[2], AABB[3], AABB[4], AABB[5]);
  lightSpaceMatrix = lightProjection * lightView;
  depthMapShader.setUniform(uniformType::mat4x4,
                            glm::value_ptr(lightSpaceMatrix),
                            "lightSpaceMatrix");
}

void ShadowMap::generate(Scene &scene) {
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  initShader(scene);
  scene.draw(depthMapShader, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // reset viewport
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::render() {
  depthMapDebugShader.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMapTexure);
  renderQuad();
}

void ShadowMap::renderQuad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    unsigned int quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
