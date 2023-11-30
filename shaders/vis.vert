#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 worldPositionFrag;

void main() {
    worldPositionFrag = vec3(M * vec4(aPos, 1.0));
    gl_Position = P*V*vec4(worldPositionFrag, 1.0);
}
