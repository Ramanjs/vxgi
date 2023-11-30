#version 440 core
#define PI 3.141592653589793
#define VOXEL_SIZE (2000/512)
#define MIPMAP_HARDCAP 6.0f

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
uniform float shininess;
uniform int hasDiffuseMap;
uniform int hasSpecularMap;
uniform int hasNormalMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
/* Material */

/* Settings */
uniform bool hasDiffuseGI;
uniform bool hasSpecularGI;
/* Settings */

out vec4 outColor;

vec3 orthogonal(vec3 u){
	u = normalize(u);
	vec3 v = vec3(0.99146, 0.11664, 0.05832); // Pick any normalized vector.
	return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}

vec3 traceDiffuseCone(const vec3 from, vec3 direction){
  direction = normalize(direction);
  const float CONE_SPREAD = 0.325;

  vec4 acc = vec4(0.0f);
  float dist = 1.0;

  while(dist < worldSizeHalf && acc.a < 1){
    vec3 c = from + dist * direction;
    vec3 coords = (c - worldCenter) / worldSizeHalf;
    coords = 0.5 * coords + 0.5;
    float l = (1 + CONE_SPREAD * dist / VOXEL_SIZE);
    float level = log2(l);
    float ll = (level + 1) * (level + 1);
    vec4 voxel = textureLod(voxelTexture, coords, min(MIPMAP_HARDCAP, level)); 
    acc += 0.075 * ll * voxel * pow(1 - voxel.a, 2);
    dist += ll * VOXEL_SIZE * 2;
	}
	return pow(acc.rgb * 2.0, vec3(1.5));
}

vec3 indirectDiffuseLight(vec3 normal){
	const float ANGLE_MIX = 0.5f; // Angle mix (1.0f => orthogonal direction, 0.0f => direction of normal).

	const float w[3] = {1.0, 1.0, 1.0};

	// Find a base for the side cones with the normal as one of its base vectors.
	const vec3 ortho = normalize(orthogonal(normal));
	const vec3 ortho2 = normalize(cross(ortho, normal));

	// Find base vectors for the corner cones too.
	const vec3 corner = 0.5f * (ortho + ortho2);
	const vec3 corner2 = 0.5f * (ortho - ortho2);

	// Find start position of trace (start with a bit of offset).
	const vec3 N_OFFSET = normal * VOXEL_SIZE;
	const vec3 C_ORIGIN = worldPosFrag + N_OFFSET;

	vec3 acc = vec3(0);

	// Trace front cone
	acc += w[0] * traceDiffuseCone(C_ORIGIN, normal);

	// Trace 4 side cones.
	const vec3 s1 = mix(normal, ortho, ANGLE_MIX);
	const vec3 s2 = mix(normal, -ortho, ANGLE_MIX);
	const vec3 s3 = mix(normal, ortho2, ANGLE_MIX);
	const vec3 s4 = mix(normal, -ortho2, ANGLE_MIX);

	acc += w[1] * traceDiffuseCone(C_ORIGIN, s1);
	acc += w[1] * traceDiffuseCone(C_ORIGIN, s2);
	acc += w[1] * traceDiffuseCone(C_ORIGIN, s3);
	acc += w[1] * traceDiffuseCone(C_ORIGIN, s4);

	// Trace 4 corner cones.
	const vec3 c1 = mix(normal, corner, ANGLE_MIX);
	const vec3 c2 = mix(normal, -corner, ANGLE_MIX);
	const vec3 c3 = mix(normal, corner2, ANGLE_MIX);
	const vec3 c4 = mix(normal, -corner2, ANGLE_MIX);

	acc += w[2] * traceDiffuseCone(C_ORIGIN, c1);
	acc += w[2] * traceDiffuseCone(C_ORIGIN, c2);
	acc += w[2] * traceDiffuseCone(C_ORIGIN, c3);
	acc += w[2] * traceDiffuseCone(C_ORIGIN, c4);

	return acc;
}

vec3 traceCone(vec3 from, vec3 direction, float aperture) {
    float max_dist = 2.0 * worldSizeHalf;
    vec4 acc       = vec4( 0.0 );

		float VoxelSize = max_dist / 512.0;
    float offset = 2.0 * VoxelSize;
    float dist   = offset + VoxelSize;

    while ( acc.a < 1.0 && dist < max_dist )
    {
        vec3 conePosition = from + direction * dist;
        float diameter    = 2.0 * aperture * dist;
        float mipLevel    = log2( diameter / VoxelSize );

        vec3 coords = ( conePosition - worldCenter ) / worldSizeHalf;
        coords      = 0.5 * coords + 0.5;

        vec4 voxel = textureLod( voxelTexture, coords, min(MIPMAP_HARDCAP, mipLevel ));
        acc += ( 1.0 - acc.a ) * voxel;

        dist += 0.5 * diameter;
    }

    return acc.rgb;
}

vec3 indirectSpecularLight(vec3 viewDirection, vec3 N) {
    float aperture = 0.0374533;

    aperture = clamp( tan( 0.5 * PI * 0.5 ), aperture, 0.5 * PI );

		const vec3 reflection = normalize(reflect(-viewDirection, N));
    vec3 specular = traceCone( worldPosFrag, reflection, 0.07 );

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

	if (hasDiffuseGI) {
		vec3 diffuseGI = color * indirectDiffuseLight(normal);
		lighting += color * diffuseGI;
	}
	if (hasSpecularGI) {
		vec3 specularGI = indirectSpecularLight(viewDir, normal);
		lighting += (specReflectivity * specularGI);
	}

  outColor = vec4(lighting, 1.0);
}
