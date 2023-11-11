#version 420 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldPosition[];
in vec3 normal[];

out vec3 worldPositionFrag;
out vec3 normalFrag;

void main() {
    // voxelize
    vec3 geometryNormal = normalize(abs(cross(worldPosition[1] - worldPosition[0], worldPosition[2] - worldPosition[0])));

    // select the dominant axis
    float axis = max(geometryNormal.x, max(geometryNormal.y, geometryNormal.z));

    for (int i = 0; i < 3; i++) {
        worldPositionFrag = worldPosition[i];
        normalFrag = normal[i];

        if (axis == geometryNormal.x) {
            gl_Position = vec4(worldPositionFrag.z, worldPositionFrag.y, worldPositionFrag.x, 1.0);
        } else if (axis == geometryNormal.y) {
            gl_Position = vec4(worldPositionFrag.x, worldPositionFrag.z, worldPositionFrag.y, 1.0);
        } else if (axis == geometryNormal.z) {
            gl_Position = vec4(worldPositionFrag.x, worldPositionFrag.y, worldPositionFrag.z, 1.0);
        }

        EmitVertex();
    }

    EndPrimitive();
}