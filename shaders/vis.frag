#version 440 core

in vec3 worldPositionFrag;
in vec3 normalFrag;

layout(RGBA8) uniform image3D voxelTexture;

out vec4 outColor;

void main() {
    // Store the voxel in the appropriate texture location
    //ivec3 voxelSize = imageSize(voxelTexture); // get the size of the voxel texture
    //vec3 normalizedFrag = (worldPositionFrag + vec3(1.0)) / 2.0; // transform the world pos from [-1, +1] to [0, 1]
    vec3 normalizedFrag = gl_FragCoord.xyz;
    normalizedFrag.z *= 512;
    ivec3 voxelPos = ivec3(normalizedFrag); // get the position of the voxel

    vec4 fColor = imageLoad(voxelTexture, voxelPos);

    outColor = fColor;
}
