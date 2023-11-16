cmake_minimum_required(VERSION 3.22)

project(stb_image)

set(STB_IMAGE_SRC
	"${PROJECT_SOURCE_DIR}/stb_image.h"
	"${PROJECT_SOURCE_DIR}/stb_image.cpp"
)

add_library(${PROJECT_NAME} STATIC ${STB_IMAGE_SRC})
