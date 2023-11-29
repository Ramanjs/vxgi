#include "camera.h"
#include "constants.h"
#include "editor.h"
#include "scene.h"
#include "shader.h"
#include "shadowmap.h"
#include "utils.h"
#include "voxelmap.h"

#include <iostream>

Camera camera(glm::vec3(100, 100, 100));

int main() {

  GLFWwindow *window = setupWindow(SCR_WIDTH, SCR_HEIGHT);

  Scene scene = Scene();
  scene.loadObj("assets/crytek-sponza/", "assets/crytek-sponza/sponza.obj");
  glm::vec3 lightPosition(200.0f, 2000.0f, 450.0f);

  ShadowMap shadowMap =
      ShadowMap("shaders/depthMap.vert", "shaders/depthMap.frag",
                "shaders/debugDepthMap.vert", "shaders/debugDepthMap.frag");

  VoxelMap voxelMap = VoxelMap(
      "shaders/voxelization.vert", "shaders/voxelization.frag",
      "shaders/voxelization.geom", "shaders/vis.vert", "shaders/vis.frag",
      "shaders/vct.vert", "shaders/vct.frag", scene, shadowMap);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Editor editor = Editor(scene, shadowMap, voxelMap, camera);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    editor.processCameraInput(window);
    editor.renderEditor();
    editor.renderScene();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
