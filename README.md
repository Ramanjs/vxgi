# interactive-global-illumination
OpenGL engine for rendering scenes with global illumination in real-time using a voxel cone tracer.

## TODO:
- [x] Refactor voxelization procedure into a separate class
- [x] Shadow map tight bound using AABB
- [x] Additional Textures
- [x] Specular Cones (kinda)
- [ ] Interactive imgui interface to edit light, material and models
- [ ] Take atomic max of a colour for each voxel
- [x] Conservative Rasterization
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

## Benchmark

The following benchmark is obtained on a single NVIDIA RTX 4090 GPU.

| VOXEL_DIM | 1280x720 | 1920x1080 |
|    ---:   |   :---:  |    ---:   |
|     512   |  54 FPS  |   54 FPS  |
|     256   |  55 FPS  |   55 FPS  |
|     128   |  56 FPS  |   56 FPS  |
|      64   |  56 FPS  |   57 FPS  |