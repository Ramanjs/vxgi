#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 worldPosition;
out vec3 normal;

void main() {
    worldPosition = vec3(M * vec4(aPos, 1.0));
    normal = normalize(mat3( transpose(inverse(M)) ) * normalize(aNorm));
    
    gl_Position = P*V* vec4(worldPosition, 1.0);
}
