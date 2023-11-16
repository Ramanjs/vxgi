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
void generateUVWBasis(glm::vec3 &u, glm::vec3 &v, glm::vec3 &w);

static void keyboardFunc(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    glm::vec3 u, v, w;
    generateUVWBasis(u, v, w);
    if (key == GLFW_KEY_W) {
      camPosition += -5.0f * w;
    } else if (key == GLFW_KEY_S) {
      camPosition += 5.0f * w;
    } else if (key == GLFW_KEY_A) {
      camPosition += 5.0f * u;
    } else if (key == GLFW_KEY_D) {
      camPosition += -5.0f * u;
    } else if (key == GLFW_KEY_J) {
      camPosition += 5.0f * v;
    } else if (key == GLFW_KEY_K) {
      camPosition += -5.0f * v;
    }
    setupViewTransformation();
  }
}

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

  glfwSetKeyCallback(window, keyboardFunc);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.setUniform(uniformType::mat4x4, glm::value_ptr(viewT), "V");
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
      45.0f, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 5000.0f);
}

void generateUVWBasis(glm::vec3 &u, glm::vec3 &v, glm::vec3 &w) {
  // standard algorithm
  glm::vec3 upVector(0.0f, 1.0f, 0.0f);
  w = glm::vec3(camPosition);
  w = glm::normalize(w);

  // if up vector is almost parallel, slightly modify to make non-colinear
  if (glm::length(glm::cross(upVector, w)) <= 0.001f) {
    upVector = glm::vec3(0.1f, 1.0f, 0.0f);
  }
  u = glm::normalize(glm::cross(upVector, w));

  v = glm::cross(w, u);
}
