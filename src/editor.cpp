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
  ImGui::SetNextWindowSize(ImVec2(400, 720));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::Begin("VXGI Editor");
  ImGui::Text("Move Point Light");
  ImGui::SliderFloat3("World Position", glm::value_ptr(lightPosition), -100.0f,
                      2000.0f);
  if (ImGui::IsItemEdited()) {
    revoxelize = true;
  }
  ImGui::End();
}

void Editor::renderScene() {
  if (revoxelize) {
    shadowMap.generate(scene, lightPosition);
    voxelmap.voxelize(lightPosition);
    revoxelize = false;
  }
  voxelmap.render(camera, lightPosition);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
