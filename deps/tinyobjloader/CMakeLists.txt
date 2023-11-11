cmake_minimum_required(VERSION 3.22)

project(tinyobjloader)

set(TINYOBJLOADER_SRC
	"${PROJECT_SOURCE_DIR}/tinyobjloader.h"
	"${PROJECT_SOURCE_DIR}/tinyobjloader.cpp"
)

add_library(${PROJECT_NAME} STATIC ${TINYOBJLOADER_SRC})
