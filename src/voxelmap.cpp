#include "voxelmap.h"
#include "constants.h"

void VoxelMap::initTexture() {
  glGenTextures(1, &voxelTexture);
  glBindTexture(GL_TEXTURE_3D, voxelTexture);
  glTexStorage3D(GL_TEXTURE_3D, 7, GL_RGBA8, VOXEL_DIM, VOXEL_DIM, VOXEL_DIM);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, VOXEL_DIM, VOXEL_DIM, VOXEL_DIM, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  // LOD settings for mipmapping.
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_3D, 0);
}

void VoxelMap::clear() {
  GLuint clearColor = 0;
  glBindTexture(GL_TEXTURE_3D, voxelTexture);
  glClearTexImage(voxelTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, &clearColor);
}

void VoxelMap::voxelize(glm::vec3 lightPosition, glm::vec3 lightColor,
                        int hasShadows) {
  clear();

  glm::mat4 modelT = glm::mat4(1.0f);
  glm::vec3 worldCenter = scene.getWorldCenter();
  float worldSizeHalf = 0.5f * scene.getWorldSize();
  GLuint voxelTextureUnit = 0;
  GLuint shadowMapUnit = 1;

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  voxelizeShader.use();
  glBindImageTexture(0, voxelTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
  voxelizeShader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  voxelizeShader.setUniform(uniformType::i1, &voxelTextureUnit, "voxelTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, voxelTexture);

  GLuint shads = hasShadows;
  voxelizeShader.setUniform(uniformType::i1, &shads, "hasShadows");
  voxelizeShader.setUniform(uniformType::fv3, glm::value_ptr(worldCenter),
                            "worldCenter");
  voxelizeShader.setUniform(uniformType::f1, &worldSizeHalf, "worldSizeHalf");
  voxelizeShader.setUniform(uniformType::fv3, glm::value_ptr(lightPosition),
                            "lightPosition");
  voxelizeShader.setUniform(uniformType::fv3, glm::value_ptr(lightColor),
                            "lightColor");
  voxelizeShader.setUniform(uniformType::mat4x4,
                            glm::value_ptr(shadowMap.getLightSpaceMatrix()),
                            "lightSpaceMatrix");
  voxelizeShader.setUniform(uniformType::i1, &shadowMapUnit, "shadowMap");

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthMapTexture());

  glViewport(0, 0, VOXEL_DIM, VOXEL_DIM);

  scene.draw(voxelizeShader, 2);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glGenerateMipmap(GL_TEXTURE_3D);

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void VoxelMap::render(Camera &camera, glm::vec3 lightPosition,
                      glm::vec3 lightColor) {
  glm::mat4 modelT = glm::mat4(1.0f);
  glm::vec3 worldCenter = scene.getWorldCenter();
  GLfloat worldSizeHalf = scene.getWorldSize() / 2.0;
  glm::vec3 camPosition = camera.position;
  glm::mat4 viewT = camera.getViewMatrix();
  glm::mat4 projectionT = glm::perspective(
      glm::radians(camera.zoom),
      (GLfloat)VIEWPORT_WIDTH / (GLfloat)VIEWPORT_HEIGHT, 0.1f, 5000.0f);
  renderShader.use();
  renderShader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  renderShader.setUniform(uniformType::fv3, glm::value_ptr(lightPosition),
                          "lightPosition");
  renderShader.setUniform(uniformType::fv3, glm::value_ptr(lightColor),
                          "lightColor");
  renderShader.setUniform(uniformType::fv3, glm::value_ptr(worldCenter),
                          "worldCenter");
  renderShader.setUniform(uniformType::f1, &worldSizeHalf, "worldSizeHalf");
  renderShader.setUniform(uniformType::mat4x4,
                          glm::value_ptr(shadowMap.getLightSpaceMatrix()),
                          "lightSpaceMatrix");
  renderShader.setUniform(uniformType::fv3, glm::value_ptr(camPosition),
                          "camPosition");
  renderShader.setUniform(uniformType::mat4x4, glm::value_ptr(viewT), "V");
  renderShader.setUniform(uniformType::mat4x4, glm::value_ptr(projectionT),
                          "P");

  GLuint voxelTextureUnit = 0;
  renderShader.setUniform(uniformType::i1, &voxelTextureUnit, "voxelTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, voxelTexture);
  GLuint shadowMapUnit = 1;
  renderShader.setUniform(uniformType::i1, &shadowMapUnit, "shadowMap");
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthMapTexture());
  glViewport(EDITOR_WIDTH, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
  scene.draw(renderShader, 2);
  // reset viewport
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void VoxelMap::visualize(Camera &camera) {
  glm::mat4 modelT = glm::mat4(1.0f);
  glm::vec3 worldCenter = scene.getWorldCenter();
  GLfloat worldSizeHalf = scene.getWorldSize() / 2.0;

  visualizationShader.use();
  visualizationShader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT),
                                 "M");
  visualizationShader.setUniform(uniformType::fv3, glm::value_ptr(worldCenter),
                                 "worldCenter");
  visualizationShader.setUniform(uniformType::f1, &worldSizeHalf,
                                 "worldSizeHalf");
  visualizationShader.setUniform(uniformType::fv3, glm::value_ptr(worldCenter),
                                 "worldCenter");
  visualizationShader.setUniform(uniformType::f1, &worldSizeHalf,
                                 "worldSizeHalf");
  glm::mat4 viewT = camera.getViewMatrix();
  visualizationShader.setUniform(uniformType::mat4x4, glm::value_ptr(viewT),
                                 "V");

  glm::mat4 projectionT =
      glm::perspective(glm::radians(camera.zoom),
                       (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 5000.0f);
  visualizationShader.setUniform(uniformType::mat4x4,
                                 glm::value_ptr(projectionT), "P");
  GLuint voxelTextureUnit = 0;
  visualizationShader.setUniform(uniformType::i1, &voxelTextureUnit,
                                 "voxelTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, voxelTexture);
  scene.draw(visualizationShader, 1);
}
