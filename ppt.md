---
marp: true
theme: uncover
class: invert
---

## Real Time Global Illumination
### (using Voxel Cone Tracing)

---

## Introduction

* Raytracing is slow
* Optimizations:
    * Brute Force: more computing power!
    * Approximation: cone tracing

---

## Motivation

* Video Games!

---

## VXGI Pipeline

* Voxelization using the GPU Hardware Rasterizer
* Hybrid Rendering Pipeline
    * Rasterized primary rays for direct lighting
    * Cone Traced secondary rays for indirect GI

---

# VOXELIZATION

![height:5in](images/ppt-voxels.jpg)

---

## One Pass Voxelization Pipeline

1. Rasterization
2. Dominant Axis Projection
3. 3D Texture Storage and Mipmapping

---

### Rasterization

* Use GPU to generate voxels
* Thin surface voxelization of triangle B with voxel V:
    * B’s plane intersects V
    * 2D projection of B along it's dominant axis intersects the 2D projection of V.
* Disable depth testing (z-buffer)

---

### Dominant Axis Selection

![](images/ppt-das.png)
$$l_{\{x,y,z\}} = |n · v_{\{x,y,z\}} |$$

---

### 3D Texture and Mipmapping

* `imageStore()` (OpenGL 4.2+) to write direct lighting to 3D image
* `glGenerateMipmap()` for automatic mipmap generation

---

# CONE TRACING

---