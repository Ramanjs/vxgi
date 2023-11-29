#include "editor.h"

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
