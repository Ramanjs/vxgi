#version 440 core

in vec3 worldPositionFrag;
in vec3 normalFrag;
in vec2 texCoordFrag;
in vec4 lightSpacePosFrag;

struct Material {
	vec3 kd;
	vec3 ks;
	float shininess;
};

uniform vec3 lightPosition;
uniform vec3 worldCenter;
uniform float worldSizeHalf;

layout(RGBA8) uniform image3D voxelTexture;
uniform sampler2D shadowMap;
uniform sampler2D diffuseMap;
uniform Material material;

float shadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
      for(int y = -1; y <= 1; ++y) {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
      }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    vec3 color = texture(diffuseMap, texCoordFrag).rgb;
    vec3 normal = normalize(normalFrag);
    vec3 lightColor = vec3(1.0);

    // diffuse
    vec3 lightDir = normalize(lightPosition - worldPositionFrag);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // calculate shadow
    float shadow = shadowCalculation(lightSpacePosFrag, lightDir, normal);
    vec3 lighting = (1.0 - shadow) * diffuse * color;

    vec3 voxel = (worldPositionFrag - worldCenter) / worldSizeHalf; // [-1, 1]
    voxel = 0.5 * voxel + vec3(0.5); // [0, 1]
    ivec3 dim = imageSize(voxelTexture);
    ivec3 coord = ivec3(dim * voxel);
    imageStore(voxelTexture, coord, vec4(lighting, 1.0));
}
