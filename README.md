# jjyouLib
My personal library for developing small projects

## Dependencies

- [Eigen](http://eigen.tuxfamily.org/)
- [GLFW](https://www.glfw.org/) (for `gl` and `vk` modules)
- [GLAD](https://glad.dav1d.de/) (for `gl` module)
- [Dear ImGui](https://github.com/ocornut/imgui) (for `gl` and `vk` modules)
- [CUDA Toolkit](https://developer.nvidia.com/cuda-toolkit) (for `cuda` module)

## Structure

All classes and functions in this library are defined in `jjyou` namespace. Currently, the library are header-only.

- `cuda`

  For developing programs based on CUDA.

- `geo`

  For geometry processing.

- `gl`

  For developing applications based on OpenGL.

- `glsl`

  A vector-math library similar to [GLM](https://github.com/g-truc/glm), but implemented in a modern way. Instead of making many specializations for `vec` and `mat` and re-defining and re-implementing their member functions, I implement them in two template class without specializations, using `requires` keyword (in C++20) if the member functions depend on the template parameters.

  Note that the classes and functions in `jjyou::glsl` are highly limited (and also, without SIMD optimization), only suitable for GLSL-related computations. Please use Eigen for scientific computing.

- `io`

  For file IO operations.

- `vk`

  For developing applications based on Vulkan. It provides wrapper classes that wrap Vulkan objects to simply the verbose Vulkan initialization process.
