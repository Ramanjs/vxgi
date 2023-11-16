#include "scene.h"
#include "stb_image.h"

#include <iostream>

void Scene::loadObj(const char *textureDir, const char *filePath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;

  std::string warn, err;
  bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                  filePath, textureDir);

  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << err << std::endl;
  }

  if (!success) {
    std::cout << "Failed to load: " << filePath << std::endl;
    exit(1);
  }

  stbi_set_flip_vertically_on_load(true);
  for (size_t m = 0; m < materials.size(); m++) {
    tinyobj::material_t *mp = &materials[m];

    if (mp->diffuse_texname.length() == 0)
      continue;
    if (textures.find(mp->diffuse_texname) != textures.end())
      continue;

    GLuint textureId;
    int w, h;
    int comp;

    std::string texturePath = textureDir;
    texturePath += mp->diffuse_texname;
    std::cout << "Texture path: " << texturePath << std::endl;
    unsigned char *image =
        stbi_load(texturePath.c_str(), &w, &h, &comp, STBI_default);
    if (!image) {
      std::cout << "Unable to load texture: " << mp->diffuse_texname
                << std::endl;
      exit(1);
    }

    std::cout << "Loaded texture: " << mp->diffuse_texname << ", w = " << w
              << ", h = " << h << ", comp = " << comp << std::endl;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (comp == 3) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,
                   image);
    } else if (comp == 4) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, image);
    } else {
      std::cout << "Comp invalid" << std::endl;
      exit(1);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
    textures.insert(std::make_pair(mp->diffuse_texname, textureId));
  }

  for (size_t s = 0; s < shapes.size(); s++) {
    Mesh mesh;
    std::vector<GLfloat> buffer;

    for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
      tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
      tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
      tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

      int current_material_id = shapes[s].mesh.material_ids[f];
      GLfloat v[3][3];
      for (int k = 0; k < 3; k++) {
        int f0 = idx0.vertex_index;
        int f1 = idx1.vertex_index;
        int f2 = idx2.vertex_index;

        v[0][k] = attrib.vertices[3 * f0 + k];
        v[1][k] = attrib.vertices[3 * f1 + k];
        v[2][k] = attrib.vertices[3 * f2 + k];
      }

      GLfloat n[3][3];
      int nf0 = idx0.normal_index;
      int nf1 = idx1.normal_index;
      int nf2 = idx2.normal_index;

      for (int k = 0; k < 3; k++) {
        n[0][k] = attrib.normals[3 * nf0 + k];
        n[1][k] = attrib.normals[3 * nf1 + k];
        n[2][k] = attrib.normals[3 * nf2 + k];
      }

      GLfloat tc[3][2];
      if (attrib.texcoords.size() > 0) {
        tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
        tc[0][1] = attrib.texcoords[2 * idx0.texcoord_index + 1];
        tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
        tc[1][1] = attrib.texcoords[2 * idx1.texcoord_index + 1];
        tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
        tc[2][1] = attrib.texcoords[2 * idx2.texcoord_index + 1];
      } else {
        tc[0][0] = 0.0f;
        tc[0][1] = 0.0f;
        tc[1][0] = 0.0f;
        tc[1][1] = 0.0f;
        tc[2][0] = 0.0f;
        tc[2][1] = 0.0f;
      }

      for (int k = 0; k < 3; k++) {
        buffer.push_back(v[k][0]);
        buffer.push_back(v[k][1]);
        buffer.push_back(v[k][2]);
        buffer.push_back(n[k][0]);
        buffer.push_back(n[k][1]);
        buffer.push_back(n[k][2]);
        buffer.push_back(tc[k][0]);
        buffer.push_back(tc[k][1]);
      }
    }

    mesh.vao = 0;
    mesh.vbo = 0;
    mesh.materialId = shapes[s].mesh.material_ids[0];
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * buffer.size(), &buffer[0],
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void *)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void *)(6 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << shapes[s].name << " triangles: " << mesh.numTriangles
              << std::endl;
    mesh.numTriangles = buffer.size() / (3 + 3 + 2) / 3;

    meshes.push_back(mesh);
  }
}

void Scene::draw(Shader &shader) {
  shader.use();

  for (size_t i = 0; i < meshes.size(); i++) {
    Mesh mesh = meshes[i];

    glBindVertexArray(mesh.vao);
    glBindTexture(GL_TEXTURE_2D, 0);
    if ((mesh.materialId < materials.size())) {
      std::string diffuse_texname = materials[mesh.materialId].diffuse_texname;
      if (textures.find(diffuse_texname) != textures.end()) {
        // std::cout << "Binded texture: " << diffuse_texname << std::endl;
        glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
      }
    }

    glDrawArrays(GL_TRIANGLES, 0, 3 * mesh.numTriangles);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
  }
}
