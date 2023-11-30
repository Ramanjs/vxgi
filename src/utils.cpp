#include "utils.h"

int openGLInit() {
  bool err = glewInit() != GLEW_OK;

  if (err) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    exit(1);
  }

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Enable multisampling
  glEnable(GL_MULTISAMPLE);

  // Enable Blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set point size
  glPointSize(10.0f);

  // Enable smooth point rendering
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  return 0;
}

static void glfwErrorCallback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow *setupWindow(int width, int height) {
  // Setup window
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit())
    exit(1);

  // Create window with graphics context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                 GL_TRUE); // Create a forward-compatible context (for macOS)

  GLFWwindow *window = glfwCreateWindow(width, height, "VXGI", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(1);
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSwapInterval(1); // Enable vsync

  // Initialize OpenGL loader
  int status = openGLInit();
  if (!status) {
    std::cout << "Initialized OpenGL succesfully " << std::endl;
  }
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

  if (GLEW_NV_conservative_raster) {
    glEnable(0x9346);
    std::cout << "Hardware conservative rasterization enabled" << std::endl;
  }

  const char *glsl_version = "#version 440";
  // Setup Dear ImGui context
  // IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return window;
}
