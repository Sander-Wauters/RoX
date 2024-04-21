# Roadmap

## Major

-   [ ] Add models.
    -   [x] Refactor Mesh to have sub-meshes.
    -   [x] Make sub-meshes shareable among meshes.
    -   [ ] Add rigid body animation
    -   [ ] Add skinned animation.

## Minor

-   [ ] Add Material properties. (diffuse, roughness, fog, wireframe, ...)
-   [ ] Add lighting controls.
-   [x] Remove Type from Mesh and create a factory for this.
-   [ ] Refactor Outline so it hold the points.
-   [ ] Vertices and indices are currently stored twice in memory. Once in Submesh and once in SubmeshDeviceData.
        The client should be able to decide wether or not to keep the data in Submesh after it is loaded into SubmeshDeviceData.

# External libraries

Clone the following libraries in the **Lib** directory.

-   [DirectXTK12](https://github.com/microsoft/DirectXTK12)
-   [Open Asset Import Library](https://github.com/assimp/assimp)

