#include "camera.h"
#include "constants.h"
#include "scene.h"
#include "shader.h"
#include "utils.h"

#include <iostream>

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xposIn, double yposIn);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void renderQuad();

glm::mat4 modelT;

Camera camera(glm::vec3(100, 100, 100));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

void setupModelTransformation();

int main() {

  GLFWwindow *window = setupWindow(SCR_WIDTH, SCR_HEIGHT);

  Shader shader = Shader("shaders/scene.vert", "shaders/scene.frag");
  shader.use();

  setupModelTransformation();
  shader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");

  Scene scene = Scene();
  scene.loadObj("assets/crytek-sponza/", "assets/crytek-sponza/sponza.obj");

  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  GLuint depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  GLuint depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glm::vec3 lightPosition(100.0f, 2000.0f, 150.0f);

  float near_plane = 1.0f, far_plane = 5000.0f;
  glm::mat4 lightProjection =
      glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane);

  Shader depthShader = Shader("shaders/depthMap.vert", "shaders/depthMap.frag");
  depthShader.use();
  depthShader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  glm::mat4 lightView =
      glm::lookAt(glm::vec3(lightPosition), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  glm::mat4 lightSpaceMatrix = lightProjection * lightView;
  depthShader.setUniform(uniformType::mat4x4, glm::value_ptr(lightSpaceMatrix),
                         "lightSpaceMatrix");

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  scene.draw(depthShader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // reset viewport
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Shader debugDepthShader =
  // Shader("shaders/debugDepthMap.vert", "shaders/debugDepthMap.frag");

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // debugDepthShader.use();
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, depthMap);
    // renderQuad();

    shader.use();
    glm::mat4 viewT = camera.getViewMatrix();
    shader.setUniform(uniformType::mat4x4, glm::value_ptr(viewT), "V");

    glm::mat4 projectionT = glm::perspective(
        glm::radians(camera.zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT,
        0.1f, 5000.0f);
    shader.setUniform(uniformType::mat4x4, glm::value_ptr(projectionT), "P");
    shader.setUniform(uniformType::mat4x4, glm::value_ptr(lightSpaceMatrix),
                      "lightSpaceMatrix");
    shader.setUniform(uniformType::fv3, glm::value_ptr(lightPosition),
                      "lightPosition");
    int sh = 1, tx = 0;
    shader.setUniform(uniformType::i1, &tx, "tex");
    shader.setUniform(uniformType::i1, &sh, "shadowMap");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    scene.draw(shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void setupModelTransformation() {
  // Modelling transformations (Model -> World coordinates)
  modelT = glm::translate(
      glm::mat4(1.0f),
      glm::vec3(0.0, 0.0, 0.0)); // Model coordinates are the world coordinates
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(Camera_Movement::FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(Camera_Movement::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(Camera_Movement::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(Camera_Movement::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera.processKeyboard(Camera_Movement::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera.processKeyboard(Camera_Movement::DOWN, deltaTime);
}

void mouseCallback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.processMouseScroll(static_cast<float>(yoffset));
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
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
