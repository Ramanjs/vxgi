#include "OBJ_Loader.h"
#include "constants.h"
#include "shader.h"
#include "utils.h"

#include <iostream>

void processInput(GLFWwindow *window);

glm::mat4 modelT, viewT, projectionT;

void setupModelTransformation();
void setupViewTransformation();
void setupProjectionTransformation();

void renderCubes(GLubyte *imageData, Shader &emitShader, GLuint cubeVAO,
                 GLuint meshSize) {
  int w, h, d;
  w = h = d = SCR_WIDTH;

  int idx = 0;
  glm::vec4 emitColor;
  glm::vec3 offset(1.0f / w, 1.0f / w, 1.0f / w);
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      for (int z = 0; z < d; z++) {
        idx = 4 * (x + y * w + z * w * w);
        float alpha = (float)imageData[idx + 3];
        /*        if (imageData[idx] > 0.0f) {*/
        /*std::cout << "Red: " << imageData[idx] << std::endl;*/
        /*}*/
        /*if (imageData[idx + 1] > 0.0f) {*/
        /*std::cout << "Green: " << imageData[idx + 1] << std::endl;*/
        /*}*/
        /*if (imageData[idx + 2] > 0.0f) {*/
        /*std::cout << "Blue: " << imageData[idx + 2] << std::endl;*/
        /*}*/

        if (alpha > 0.0f) {
          emitColor = glm::vec4(((float)imageData[idx]) / 255.0f,
                                ((float)imageData[idx + 1]) / 255.0f,
                                ((float)imageData[idx + 2]) / 255.0f, 1.0f);
          glm::vec3 pos =
              offset + glm::vec3(x * (2.0 / w), y * (2.0 / w), z * (2.0 / w)) +
              glm::vec3(-1.0f);
          modelT = glm::translate(glm::mat4(1.0f), pos) *
                   glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / w));

          emitShader.use();
          emitShader.setUniform(uniformType::mat4x4, glm::value_ptr(modelT),
                                "M");
          emitShader.setUniform(uniformType::fv4, glm::value_ptr(emitColor),
                                "emitColor");
          glBindVertexArray(cubeVAO);
          glDrawElements(GL_TRIANGLES, meshSize, GL_UNSIGNED_INT, 0);
        }
      }
    }
  }
}

int main() {

  GLFWwindow *window = setupWindow(SCR_WIDTH, SCR_HEIGHT);
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

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

  /* VOXELIZE */
  glm::vec3 lightPosition(10, 10, 10);
  glm::vec3 camPosition(0, 0, 10);

  // init shader
  Shader shader =
      Shader("./shaders/voxelization.vert", "./shaders/voxelization.frag");
  shader.use();

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

  shader.setUniform(uniformType::fv3, glm::value_ptr(lightPosition),
                    "lightPosition");
  shader.setUniform(uniformType::fv3, glm::value_ptr(camPosition),
                    "camPosition");
  int vt = 0;
  shader.setUniform(uniformType::i1, &vt, "voxelTexture");

  // create texture for voxelization
  GLuint voxelTexture;
  std::vector<GLubyte> clearBuffer(4 * SCR_WIDTH * SCR_WIDTH * SCR_WIDTH, 0);

  glGenTextures(1, &voxelTexture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, voxelTexture);
  glTexStorage3D(GL_TEXTURE_3D, 7, GL_RGBA8, SCR_WIDTH, SCR_WIDTH, SCR_WIDTH);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, SCR_WIDTH, SCR_WIDTH, SCR_WIDTH, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, &clearBuffer[0]);

  // LOD settings for mipmapping.
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glGenerateMipmap(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, 0);

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glBindImageTexture(0, voxelTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, mesh.Indices.size(), GL_UNSIGNED_INT, 0);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glGenerateMipmap(GL_TEXTURE_3D);

  // TODO: fetch from GPU
  GLubyte *imageData = (GLubyte *)malloc(sizeof(GLubyte) * 4 * SCR_WIDTH *
                                         SCR_WIDTH * SCR_WIDTH);
  glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

  // int w, h, d;
  // w = h = d = SCR_WIDTH;

  // int idx = 0;
  // glm::vec4 emitColor;
  // for (int x = 0; x < w; x++) {
  // for (int y = 0; y < h; y++) {
  // for (int z = 0; z < d; z++) {
  // idx = 4 * (x + y * w + z * w * w);
  // float alpha = (float)imageData[idx + 3];
  // if ((float)imageData[idx] > 0.0f) {
  // std::cout << "Red: " << (float)imageData[idx] << std::endl;
  //}
  // if ((float)imageData[idx + 1] > 0.0f) {
  // std::cout << "Green: " << (float)imageData[idx + 1] << std::endl;
  //}
  // if ((float)imageData[idx + 2] > 0.0f) {
  // std::cout << "Blue: " << (float)imageData[idx + 2] << std::endl;
  //}

  // if (alpha > 0.0f) {
  // std::cout << "Alpha: " << (float)imageData[idx + 3] << std::endl;
  //}
  //}
  //}
  //}
  /* VOXELIZE */

  /* VISUALIZATION */
  // load cube from obj file
  /*  loaded = loader.LoadFile("assets/cube.obj");*/
  /*if (!loaded) {*/
  /*std::cout << "Failed to load mesh" << std::endl;*/
  /*exit(1);*/
  /*}*/

  /*Shader emitShader = Shader("shaders/emit.vert", "shaders/emit.frag");*/
  /*emitShader.use();*/

  /*// make VOXEL_DIM^3 cubes*/
  /*mesh = loader.LoadedMeshes[0];*/
  /*std::cout << mesh.MeshName << std::endl;*/
  /*GLfloat *cubeV = new GLfloat[3 * mesh.Vertices.size()];*/
  /*GLuint *cubeI = new GLuint[mesh.Indices.size()];*/

  /*for (int i = 0; i < mesh.Vertices.size(); i++) {*/
  /*cubeV[3 * i] = mesh.Vertices[i].Position.X;*/
  /*cubeV[3 * i + 1] = mesh.Vertices[i].Position.Y;*/
  /*cubeV[3 * i + 2] = mesh.Vertices[i].Position.Z;*/
  /*}*/

  /*for (int i = 0; i < mesh.Indices.size(); i++) {*/
  /*cubeI[i] = mesh.Indices[i];*/
  /*}*/

  /*GLuint cubeVAO, cubeVBO, cubeEBO;*/
  /*glGenVertexArrays(1, &cubeVAO);*/
  /*glGenBuffers(1, &cubeVBO);*/
  /*glGenBuffers(1, &cubeEBO);*/

  /*glBindVertexArray(cubeVAO);*/

  /*glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);*/
  /*glBufferData(GL_ARRAY_BUFFER, sizeof(cubeV), cubeV, GL_STATIC_DRAW);*/
  /*glEnableVertexAttribArray(0);*/
  /*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);*/

  /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);*/
  /*glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeI), cubeI,
   * GL_STATIC_DRAW);*/

  /*glBindBuffer(GL_ARRAY_BUFFER, 0);*/
  /*glBindVertexArray(0);*/
  /* VISUALIZATION */

  Shader vis = Shader("shaders/voxelization.vert", "shaders/vis.frag");
  vis.use();
  vis.setUniform(uniformType::mat4x4, glm::value_ptr(modelT), "M");
  vis.setUniform(uniformType::mat4x4, glm::value_ptr(viewT), "V");
  vis.setUniform(uniformType::mat4x4, glm::value_ptr(projectionT), "P");
  vis.setUniform(uniformType::i1, &vt, "voxelTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, voxelTexture);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vis.use();
    glBindTexture(GL_TEXTURE_3D, voxelTexture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.Indices.size(), GL_UNSIGNED_INT, 0);

    // renderCubes(imageData, emitShader, cubeVAO, mesh.Indices.size());

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
