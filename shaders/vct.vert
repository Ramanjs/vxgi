#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBit;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 lightSpaceMatrix;

out vec3 worldPosFrag;
out vec3 normalFrag;
out vec2 texCoordFrag;
out mat3 TBN;
out vec4 lightSpacePosFrag;

void main() {
    worldPosFrag = vec3(M * vec4(aPos, 1.0));
    normalFrag = transpose(inverse(mat3(M))) * aNorm;
    lightSpacePosFrag = lightSpaceMatrix * vec4(worldPosFrag, 1.0);
    texCoordFrag = aTex;
    TBN = mat3(aTan, aBit, aNorm);
    
    gl_Position = P*V*vec4(worldPosFrag, 1.0);
}
