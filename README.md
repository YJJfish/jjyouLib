# jjyouLib
My personal C++ library for developing projects.

## Dependencies

- [Eigen](http://eigen.tuxfamily.org/)
- [GLFW](https://www.glfw.org/) (for `gl` and `vk` modules)
- [GLAD](https://glad.dav1d.de/) (for `gl` module)
- [Dear ImGui](https://github.com/ocornut/imgui) (for `gl` and `vk` modules)
- [CUDA Toolkit](https://developer.nvidia.com/cuda-toolkit) (for `cuda` module)

## Structure

The library is header-only. All classes and functions are defined in the namespace `jjyou`, with subsequent namespaces same as the folder structure. 

- `cuda`

  For developing programs based on CUDA.

  - `DeviceArray1D`, `DeviceArray2D`

- `geo`

  For geometry processing.

  - `HalfedgeMesh`
  - `IndexedMesh`

- `gl`

  For developing applications based on OpenGL.

- `glsl`

  A vector-math library similar to [GLM](https://github.com/g-truc/glm), for performing vector and matrix computations with the same name convention of [GLSL](https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language).

- `io`

  For file IO operations.

  - `Json`
  - `PlyFile`
  - `IniFile`
  - `ArgParser`

- `vk`

  For developing applications based on Vulkan. It provides wrapper classes that wrap Vulkan objects to simply the verbose Vulkan initialization process.
  
  - `Instance`, `InstanceBuilder`
  - `PhysicalDevice`, `PhysicalDeviceSelector`
  - `Device`, `DeviceBuilder`
  - `Swapchain`, `SwapchainBuilder`
  - `Memory`, `MemoryAllocator`
  - `Texture2D`
  
- `vis`

  For visualization. Can be used together with `gl` or `vk` module. Before including this library, you must define `JJYOU_USE_OPENGL` or `JJYOU_USE_VULKAN`.

  - `CameraView`, `FirstPersonView`, `SceneView`

