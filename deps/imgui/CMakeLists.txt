cmake_minimum_required(VERSION 3.22)

project(imgui)

set(IMGUI_SRC
	"${PROJECT_SOURCE_DIR}/imgui_impl_glfw.cpp"
	"${PROJECT_SOURCE_DIR}/imgui_impl_opengl3.cpp"
	"${PROJECT_SOURCE_DIR}/imgui.cpp"
	"${PROJECT_SOURCE_DIR}/imgui_demo.cpp"
	"${PROJECT_SOURCE_DIR}/imgui_draw.cpp"
	"${PROJECT_SOURCE_DIR}/imgui_widgets.cpp"
)

add_library(${PROJECT_NAME} STATIC ${IMGUI_SRC})
