#version 440 core

in vec3 worldPositionFrag;
in vec3 normalFrag;

uniform vec3 camPosition;
uniform vec3 lightPosition;

layout(RGBA8) uniform image3D voxelTexture;

out vec4 outColor;

void main() {
    vec3 Ls = vec3(1.0, 1.0, 1.0);
    vec3 Ld = vec3(0.7, 0.7, 0.7);
    vec3 La = vec3(0.6, 0.3, 0.4);

    vec3 ks = vec3(0.5, 0.5, 0.5);
    vec3 kd = vec3(0.5, 0.6, 0.4);
    vec3 ka = vec3(1.0, 1.0, 1.0);

    float spec_exp = 32;

    // ambient
    vec3 Ia = ka * La;

    // diffuse
    vec3 norm = normalize(normalFrag);
    vec3 lDir = normalize(lightPosition - worldPositionFrag);
    vec3 Id = kd * max(dot(lDir, norm) * Ld, 0);

    // specular
    vec3 v = normalize(camPosition - worldPositionFrag);
    vec3 r = reflect(-lDir, norm);
    vec3 Is = ks * Ls * max(pow(max(dot(v, r), 0), spec_exp), 0);

    vec3 fColor = Ia + Id + Is;




    // Store the voxel in the appropriate texture location
    //ivec3 voxelSize = imageSize(voxelTexture); // get the size of the voxel texture
    //vec3 normalizedFrag = (worldPositionFrag + vec3(1.0)) / 2.0; // transform the world pos from [-1, +1] to [0, 1]
    vec3 normalizedFrag = gl_FragCoord.xyz;
    normalizedFrag.z *= 512;
    ivec3 voxelPos = ivec3(normalizedFrag); // get the position of the voxel

    if (200 <= voxelPos.x && voxelPos.x < 512 && 200 <= voxelPos.y && voxelPos.y < 512 && 200 <= voxelPos.z && voxelPos.z < 512) {
        outColor = vec4(fColor, 1.0);
    } else {
        outColor = vec4(0, 0, 0, 1);
    }
    imageStore(voxelTexture, voxelPos, outColor);
}
