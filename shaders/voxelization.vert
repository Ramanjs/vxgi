#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

uniform mat4 M;
uniform mat4 lightSpaceMatrix;

out vec3 worldPositionGeom;
out vec3 normalGeom;
out vec2 texCoordGeom;
out vec4 lightSpacePosGeom;

void main() {
    worldPositionGeom = vec3(M * vec4(aPos, 1.0));
    normalGeom = transpose(inverse(mat3(M))) * aNorm;
    lightSpacePosGeom = lightSpaceMatrix * vec4(worldPositionGeom, 1.0);
    texCoordGeom = aTex;
    gl_Position = vec4(worldPositionGeom, 1.0);
}
