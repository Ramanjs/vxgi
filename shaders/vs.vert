#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 worldPositionFrag;
out vec3 normalFrag;

void main() {
    gl_Position = P*V*M * vec4(aPos, 1.0);
    worldPositionFrag = vec3(M * vec4(aPos, 1.0));
    normalFrag = normalize(mat3(transpose(inverse(M))) * normalize(aNorm));
}
