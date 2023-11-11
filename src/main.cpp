#include "OBJ_Loader.h"
#include "shader.h"
#include "utils.h"

#include <iostream>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::mat4 modelT, viewT, projectionT;

void setupModelTransformation();
void setupViewTransformation();
void setupProjectionTransformation();

int main() {

  GLFWwindow *window = setupWindow(SCR_WIDTH, SCR_HEIGHT);

  // shaders
  // Shader shader = Shader("./shaders/vs.vert", "./shaders/fsociety.frag", "./shaders/gshader.gs");
  // shader.use();

  Shader shader = Shader("./shaders/voxelization.vert", "./shaders/voxelization.frag", "./shaders/voxelization.gs");
  shader.use();

  objl::Loader loader = objl::Loader();
  bool loaded = loader.LoadFile("assets/bunny.obj");

  if (!loaded) {
    std::cout << "Failed to load mesh" << std::endl;
    exit(1);
  }
  objl::Mesh mesh = loader.LoadedMeshes[0];

  float vertices[3 * mesh.Vertices.size()];
  float normals[3 * mesh.Vertices.size()];
  GLuint indices[mesh.Indices.size()];

  for (int i = 0; i < mesh.Vertices.size(); i++) {
    vertices[3 * i] = mesh.Vertices[i].Position.X * 2;
    vertices[3 * i + 1] = mesh.Vertices[i].Position.Y * 2;
    vertices[3 * i + 2] = mesh.Vertices[i].Position.Z * 2;
    normals[3 * i] = mesh.Vertices[i].Normal.X;
    normals[3 * i + 1] = mesh.Vertices[i].Normal.Y;
    normals[3 * i + 2] = mesh.Vertices[i].Normal.Z;
  }

  for (int i = 0; i < mesh.Indices.size(); i++) {
    indices[i] = mesh.Indices[i];
  }

  // VBO, VAO
  GLuint VBO, VAO, EBO, nVBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &nVBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  // Copying vertex data to VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  // Copying normal data to nVBO
  glBindBuffer(GL_ARRAY_BUFFER, nVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  setupModelTransformation();
  setupViewTransformation();
  setupProjectionTransformation();

  shader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  shader.setUniform(uniformType::mat4x4, glm::value_ptr(viewT), "V");
  shader.setUniform(uniformType::mat4x4, glm::value_ptr(projectionT), "P");

  // create texture for voxelization
  GLuint voxelTexture;
  glGenTextures(1, &voxelTexture);
  glBindTexture(GL_TEXTURE_3D, voxelTexture);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 128, 128, 128, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  shader.setUniform(uniformType::i1, 0, "voxelTexture");


  glm::vec3 lightPosition(10, 10, 10);
  glm::vec3 camPosition(0, 0, 10);
  shader.setUniform(uniformType::fv3, glm::value_ptr(lightPosition),
                    "lightPosition");
  shader.setUniform(uniformType::fv3, glm::value_ptr(camPosition),
                    "camPosition");

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.Indices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &nVBO);
  glDeleteBuffers(1, &EBO);

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
  viewT = glm::lookAt(glm::vec3(0.0, 0.0, 10.0), glm::vec3(0.0, 0.0, 0.0),
                      glm::vec3(0.0, 1.0, 0.0));
}

void setupProjectionTransformation() {
  // Projection transformation
  projectionT = glm::perspective(
      45.0f, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 1000.0f);
}
