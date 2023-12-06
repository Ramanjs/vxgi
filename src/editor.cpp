#include "editor.h"

void Editor::processCameraInput(GLFWwindow *window) {
  ImGuiIO &io = ImGui::GetIO();
  float deltaTime = io.DeltaTime;

  if (viewMode) {
    camera.processMouseMovement(io.MouseDelta.x, -io.MouseDelta.y);
    camera.processMouseScroll(io.MouseWheel);
  }

  if (ImGui::IsKeyPressed('C')) {
    if (viewMode) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    viewMode = !viewMode;
  }
  if (ImGui::IsKeyDown('W'))
    camera.processKeyboard(Camera_Movement::FORWARD, deltaTime);
  if (ImGui::IsKeyDown('S'))
    camera.processKeyboard(Camera_Movement::BACKWARD, deltaTime);
  if (ImGui::IsKeyDown('A'))
    camera.processKeyboard(Camera_Movement::LEFT, deltaTime);
  if (ImGui::IsKeyDown('D'))
    camera.processKeyboard(Camera_Movement::RIGHT, deltaTime);
  if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
    camera.processKeyboard(Camera_Movement::UP, deltaTime);
  if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
    camera.processKeyboard(Camera_Movement::DOWN, deltaTime);
  if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
    glfwSetWindowShouldClose(window, true);
}

void Editor::renderEditor() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::SetNextWindowSize(ImVec2(EDITOR_WIDTH, EDITOR_HEIGHT));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::Begin("VXGI Editor");
  if (ImGui::CollapsingHeader("Point Light")) {
    ImGui::Text("Position");
    ImGui::SliderFloat("x", glm::value_ptr(lightPosition), -500.0f, 500.0f);
    ImGui::SliderFloat("y", glm::value_ptr(lightPosition) + 1, 1200.0f,
                       2000.0f);
    ImGui::SliderFloat("z", glm::value_ptr(lightPosition) + 2, -500.0f, 500.0f);
    if (ImGui::IsItemEdited()) {
      regenShadowMap = true;
      revoxelize = true;
    }
    ImGui::ColorEdit3("Color", glm::value_ptr(lightColor));
    if (ImGui::IsItemEdited()) {
      revoxelize = true;
    }
  }

  if (ImGui::CollapsingHeader("Engine")) {
    ImGui::Text("Rendering Mode");
    if (ImGui::RadioButton("Visualize", engineMode == EngineMode::VISUALIZE)) {
      engineMode = EngineMode::VISUALIZE;
    }
    if (ImGui::RadioButton("GI", engineMode == EngineMode::RENDER)) {
      engineMode = EngineMode::RENDER;
      hasShadows = true;
      revoxelize = true;
    }
    if (engineMode == EngineMode::VISUALIZE) {
      ImGui::Separator();
      ImGui::Checkbox("Shadows", &hasShadows);
      if (ImGui::IsItemEdited()) {
        revoxelize = true;
      }
    } else {
      ImGui::Separator();
      ImGui::Checkbox("Indirect diffuse", &diffuseGI);
      ImGui::Checkbox("Indirect specular", &specularGI);
    }

    ImGui::Separator();
    ImGui::Text("Voxel Map Resolution");
    const char resolutionLabels[100] = "x512\0x256\0x128\0x64";
    if (ImGui::Combo("", &voxelRes, resolutionLabels)) {
      revoxelize = true;
      if (voxelRes == 0)
        VOXEL_DIM = 512;
      if (voxelRes == 1)
        VOXEL_DIM = 256;
      if (voxelRes == 2)
        VOXEL_DIM = 128;
      if (voxelRes == 3)
        VOXEL_DIM = 64;
      voxelmap.resizeTexture();
    }

    ImGui::Separator();
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
  }

  if (ImGui::CollapsingHeader("Models")) {
    if (ImGui::TreeNode("Floor")) {
      ImGui::SliderFloat("Specular", &floorSpecular, 0.0f, 1.0f);
      if (ImGui::IsItemEdited()) {
        floorSpecularRef = glm::vec3(floorSpecular);
      }
      ImGui::SliderFloat("Emissive", &floorEmissive, 0.0f, 1.0f);
      if (ImGui::IsItemEdited()) {
        floorEmissiveRef = glm::vec3(floorEmissive);
        revoxelize = true;
      }
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("Curtain")) {
      ImGui::SliderFloat("Specular", &curtainSpecular, 0.0f, 1.0f);
      if (ImGui::IsItemEdited()) {
        curtainSpecularRef = glm::vec3(curtainSpecular);
      }
      ImGui::SliderFloat("Emissive", &curtainEmissive, 0.0f, 1.0f);
      if (ImGui::IsItemEdited()) {
        curtainEmissiveRef = glm::vec3(curtainEmissive);
        revoxelize = true;
      }
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("Dragon")) {
      ImGui::Text("Position");
      ImGui::SliderFloat("x", glm::value_ptr(scene.dynamicMeshPosition),
                         -200.0f, 200.0f);
      ImGui::SliderFloat("y", glm::value_ptr(scene.dynamicMeshPosition) + 1,
                         -200.0f, 200.0f);
      ImGui::SliderFloat("z", glm::value_ptr(scene.dynamicMeshPosition) + 2,
                         -200.0f, 200.0f);
      if (ImGui::IsItemEdited()) {
        revoxelize = true;
      }
      ImGui::SliderFloat("Specular", &dragonSpecular, 0.0f, 1.0f);
      ImGui::SliderFloat("Emissive", &dragonEmissive, 0.0f, 1.0f);
      ImGui::TreePop();
    }
  }

  ImGui::End();
}

void Editor::renderScene() {
  if (regenShadowMap) {
    shadowMap.generate(scene, lightPosition);
    regenShadowMap = false;
  }
  if (revoxelize) {
    voxelmap.voxelize(lightPosition, lightColor, hasShadows);
    revoxelize = false;
  }
  if (engineMode == EngineMode::VISUALIZE) {
    voxelmap.visualize(camera);
  } else if (engineMode == EngineMode::RENDER) {
    voxelmap.render(camera, lightPosition, lightColor, diffuseGI, specularGI);
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
