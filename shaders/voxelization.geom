#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldPositionGeom[];
in vec3 normalGeom[];
in vec2 texCoordGeom[];
in vec4 lightSpacePosGeom[];

uniform vec3 worldCenter;
uniform float worldSizeHalf;

out vec3 worldPositionFrag;
out vec3 normalFrag;
out vec2 texCoordFrag;
out vec4 lightSpacePosFrag;

void main() {
    // voxelize
    vec3 geometryNormal = abs(cross(worldPositionGeom[1] - worldPositionGeom[0], worldPositionGeom[2] - worldPositionGeom[0]));

    // select the dominant axis
    float axis = max(geometryNormal.x, max(geometryNormal.y, geometryNormal.z));

    for (int i = 0; i < 3; i++) {
        worldPositionFrag = worldPositionGeom[i];
        normalFrag = normalGeom[i];
        texCoordFrag = texCoordGeom[i];
        lightSpacePosFrag = lightSpacePosGeom[i];
        vec3 outputPosition = (worldPositionFrag - worldCenter) / worldSizeHalf;

        if (axis == geometryNormal.x) {
            gl_Position = vec4(outputPosition.zy, 1.0, 1.0);
        } else if (axis == geometryNormal.y) {
            gl_Position = vec4(outputPosition.xz, 1.0, 1.0);
        } else if (axis == geometryNormal.z) {
            gl_Position = vec4(outputPosition.xy, 1.0, 1.0);
        }
        EmitVertex();
    }
    EndPrimitive();
}
