#include "constants.h"
#include "scene.h"
#include "shader.h"
#include "utils.h"

#include <iostream>

void processInput(GLFWwindow *window);

glm::mat4 modelT, viewT, projectionT;

glm::vec3 camPosition;

void setupModelTransformation();
void setupViewTransformation();
void setupProjectionTransformation();

int main() {

  GLFWwindow *window = setupWindow(SCR_WIDTH, SCR_HEIGHT);
  camPosition = glm::vec3(100, 100, 100);

  Shader shader = Shader("shaders/scene.vert", "shaders/scene.frag");
  shader.use();

  setupModelTransformation();
  setupViewTransformation();
  setupProjectionTransformation();
  shader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  shader.setUniform(uniformType::mat4x4, glm::value_ptr(viewT), "V");
  shader.setUniform(uniformType::mat4x4, glm::value_ptr(projectionT), "P");

  Scene scene = Scene();
  scene.loadObj("assets/crytek-sponza/", "assets/crytek-sponza/sponza.obj");

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.draw(shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void setupModelTransformation() {
  // Modelling transformations (Model -> World coordinates)
  modelT = glm::translate(
      glm::mat4(1.0f),
      glm::vec3(0.0, 0.0, 0.0)); // Model coordinates are the world coordinates
}

void setupViewTransformation() {
  viewT = glm::lookAt(camPosition, glm::vec3(0.0, 0.0, 0.0),
                      glm::vec3(0.0, 1.0, 0.0));
}

void setupProjectionTransformation() {
  // Projection transformation
  projectionT = glm::perspective(
      45.0f, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 1000.0f);
}
