#version 420 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldPositionFrag[];
in vec3 normalFrag[];

out vec3 worldPositionFrag;
out vec3 normalFrag;

void main() {
    // voxelize
    vec3 geometryNormal = normalize(abs(cross(worldPositionFrag[1] - worldPositionFrag[0], worldPositionFrag[2] - worldPositionFrag[0])));

    // select the dominant axis
    float axis = max(geometryNormal.x, max(geometryNormal.y, geometryNormal.z));

    for (int i = 0; i < 3; i++) {
        worldPositionFrag = worldPositionFrag[i];
        normalFrag = normalFrag[i];

        switch (axis) {
            case geometryNormal.x:
                gl_Position = vec4(worldPositionFrag.zyx 1.0);
                break;
            case geometryNormal.y:
                gl_Position = vec4(worldPositionFrag.xzy 1.0);
                break;
            case geometryNormal.z:
                gl_Position = vec4(worldPositionFrag.xyz 1.0);
                break;
        }

        EmitVertex();
    }

    EndPrimitive();
}