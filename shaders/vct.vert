#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 lightSpaceMatrix;

out vec3 worldPosFrag;
out vec3 normalFrag;
out vec2 texCoordFrag;
out vec4 lightSpacePosFrag;

void main() {
    worldPosFrag = vec3(M * vec4(aPos, 1.0));
    normalFrag = transpose(inverse(mat3(M))) * aNorm;
    lightSpacePosFrag = lightSpaceMatrix * vec4(worldPosFrag, 1.0);
    texCoordFrag = aTex;
    
    gl_Position = P*V*vec4(worldPosFrag, 1.0);
}
