cmake_minimum_required(VERSION 3.27)

project(imgui)

file(GLOB IMGUI_SRC
  "imgui_impl_glfw.cpp"
	"imgui_impl_opengl3.cpp"
	"imgui.cpp"
	"imgui_demo.cpp"
	"imgui_draw.cpp"
	"imgui_widgets.cpp"
  #"${PROJECT_SOURCE_DIR}/*.h"
  #"${PROJECT_SOURCE_DIR}/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${IMGUI_SRC})
