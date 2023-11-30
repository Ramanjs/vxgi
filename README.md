# interactive-global-illumination
OpenGL engine for rendering scenes with global illumination in real-time using a voxel cone tracer.

## TODO:
- [x] Refactor voxelization procedure into a separate class
- [x] Shadow map tight bound using AABB
- [x] Additional Textures
- [x] Specular Cones (kinda)
- [x] Interactive imgui interface to edit light, material and models
- [ ] Take atomic max of a colour for each voxel
- [x] Conservative Rasterization
- [ ] Compute two light bounces
- [ ] Emissive materials
- [ ] Possibly use assimp for a PBR based pipeline
- [ ] Sparse voxel octree

## ImGui Editor Options

- [x] Move point light source around and change its color
- [x] Toggle between viewing modes:
    - [x] Visualization
        - [x] Shadows
    - [x] GI
        - [x] Indirect Diffuse
        - [x] Indirect Specular
- [x] Change material properties
- [ ] Move objects around
- [x] Change voxel map resolution
