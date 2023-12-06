#version 440 core
#define MIPMAP_CAP 7.0f

in vec3 worldPosFrag;
in vec3 normalFrag;
in vec2 texCoordFrag;
in mat3 TBN;
in vec4 lightSpacePosFrag;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 camPosition;
uniform vec3 worldCenter;
uniform float worldSizeHalf;

uniform sampler3D voxelTexture;
uniform sampler2D shadowMap;

/* Material */
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 ke;
uniform float shininess;
uniform int hasDiffuseMap;
uniform int hasSpecularMap;
uniform int hasNormalMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
/* Material */

/* Settings */
uniform int VOXEL_DIM;
uniform bool hasDiffuseGI;
uniform bool hasSpecularGI;
/* Settings */

out vec4 outColor;

vec3 orthogonal(vec3 u){
	u = normalize(u);
	vec3 v = normalize(vec3(1));
	return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}

vec3 traceDiffuseCone(const vec3 from, vec3 direction){
  direction = normalize(direction);
  const float aperture = 0.767;

  vec4 acc = vec4(0.0f);
  float dist = 1.0;

	float voxelSize = 2.0 * worldSizeHalf / VOXEL_DIM;

  while(dist < worldSizeHalf && acc.a < 1){
    vec3 conePosition = from + dist * direction;
    vec3 coords = (conePosition - worldCenter) / worldSizeHalf;
    coords = 0.5 * coords + 0.5;
    float level = log2(1 + aperture * dist / voxelSize);
    float lsquared = (level + 1) * (level + 1);
    vec4 voxel = textureLod(voxelTexture, coords, min(MIPMAP_CAP, level)); 
    acc += 0.075 * lsquared * voxel * pow(1 - voxel.a, 2);
    dist += lsquared * voxelSize * 2;
	}
	return acc.rgb * 2.0;
}

vec3 indirectDiffuseLight(vec3 normal){
	const float ANGLE_MIX = 0.5f;
	const float w[3] = {1.0, 1.0, 1.0};

	const vec3 ortho = normalize(orthogonal(normal));
	const vec3 ortho2 = normalize(cross(ortho, normal));

	float voxelSize = 2.0 * worldSizeHalf / VOXEL_DIM;
	const vec3 offset = normal * voxelSize;
	const vec3 coneOrigin = worldPosFrag + offset;

	vec3 acc = vec3(0);

	// Trace front cone
	acc += w[0] * traceDiffuseCone(coneOrigin, normal);

	// Trace 4 side cones.
	const vec3 s1 = mix(normal, ortho, ANGLE_MIX);
	const vec3 s2 = mix(normal, -ortho, ANGLE_MIX);
	const vec3 s3 = mix(normal, ortho2, ANGLE_MIX);
	const vec3 s4 = mix(normal, -ortho2, ANGLE_MIX);

	acc += w[1] * traceDiffuseCone(coneOrigin, s1);
	acc += w[1] * traceDiffuseCone(coneOrigin, s2);
	acc += w[1] * traceDiffuseCone(coneOrigin, s3);
	acc += w[1] * traceDiffuseCone(coneOrigin, s4);

	return acc;
}

vec3 traceSpecularCone(vec3 from, vec3 direction, float aperture) {
    float max_dist = worldSizeHalf / 4.0;
    vec4 acc = vec4( 0.0 );

		float voxelSize = 2.0 * worldSizeHalf / VOXEL_DIM;
    float offset = 2.0 * voxelSize;
    float dist = offset + voxelSize;

    while (acc.a < 1.0 && dist < max_dist) {
        vec3 conePosition = from + direction * dist;
        float diameter = 2.0 * aperture * dist;
        float level = log2(diameter / voxelSize);

        vec3 coords = (conePosition - worldCenter) / worldSizeHalf;
        coords = 0.5 * coords + 0.5;

        vec4 voxel = textureLod(voxelTexture, coords, min(MIPMAP_CAP, level));
        acc += (1.0 - acc.a) * voxel;
        dist += 0.5 * diameter;
    }

    return acc.rgb;
}

vec3 indirectSpecularLight(vec3 viewDirection, vec3 N) {
    float aperture = 0.07;

		const vec3 reflection = normalize(reflect(-viewDirection, N));
    vec3 specular = traceSpecularCone(worldPosFrag, reflection, aperture);

    return specular;
}

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
  if (hasDiffuseMap == 0) {
    color = kd;
  }

  vec3 normal = normalize(normalFrag);
	if (hasNormalMap == 1) {
		normal = texture(normalMap, texCoordFrag).rgb;
		normal = normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
	}

  // diffuse
  vec3 lightDir = normalize(lightPosition - worldPosFrag);
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * lightColor;

	// specular
  vec3 viewDir = normalize(camPosition - worldPosFrag);
  vec3 specReflectivity = texture(specularMap, texCoordFrag).rgb;
	if (hasSpecularMap == 0) {
		specReflectivity = ks;
	}

  // calculate shadow
  float shadow = shadowCalculation(lightSpacePosFrag, lightDir, normal);
  vec3 lighting = (1.0 - shadow) * (diffuse) * color;
	lighting += ke * color;

	// diffuse GI
	if (hasDiffuseGI) {
		vec3 diffuseGI = color * indirectDiffuseLight(normal);
		lighting += diffuseGI;
	}
	// specular GI
	if (hasSpecularGI) {
		vec3 specularGI = indirectSpecularLight(viewDir, normal);
		lighting += (specReflectivity * specularGI);
	}

  outColor = vec4(lighting, 1.0);
}
