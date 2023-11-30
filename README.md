# interactive-global-illumination
OpenGL engine for rendering scenes with global illumination in real-time using a voxel cone tracer.

## TODO:
- [x] Refactor voxelization procedure into a separate class
- [x] Shadow map tight bound using AABB
- [x] Additional Textures
- [x] Specular Cones
- [x] Interactive imgui interface to edit light, material and models
- [ ] Take atomic max of a colour for each voxel
- [x] Conservative Rasterization
- [x] Emissive materials
- [ ] Dynamic mesh voxelization
- [ ] Compute two light bounces ??
- [ ] Possibly use assimp for a PBR based pipeline ??
- [ ] Sparse voxel octree ??

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

## Benchmark (needs to be redone)

The following benchmark is obtained on a single NVIDIA RTX 4090 GPU.

| VOXEL_DIM | 1280x720 | 1920x1080 |
|    ---:   |   :---:  |    ---:   |
|     512   |  54 FPS  |   54 FPS  |
|     256   |  55 FPS  |   55 FPS  |
|     128   |  56 FPS  |   56 FPS  |
|      64   |  56 FPS  |   57 FPS  |
