# interactive-global-illumination
OpenGL engine for rendering scenes with global illumination in real-time using a voxel cone tracer.

## TODO:
- [x] Refactor voxelization procedure into a separate class
- [x] Shadow map tight bound using AABB
- [x] Additional Textures
- [x] Specular Cones (kinda)
- [ ] Interactive imgui interface to edit light, material and models
- [ ] Take atomic max of a colour for each voxel
- [ ] Conservative Rasterization
- [ ] Compute two light bounces
- [ ] Emissive materials
- [ ] Possibly use assimp for a PBR based pipeline
- [ ] Sparse voxel octree

## ImGui Editor Options

- [ ] Move point light source around
- [ ] Toggle between viewing modes:
    - [ ] Visualization
        - [ ] Shadows ??
    - [ ] GI
        - [ ] Indirect Diffuse
        - [ ] Indirect Specular
- [ ] Change material properties
- [ ] Move objects around
- [ ] Change voxel map resolution
