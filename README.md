# OGLFramework_uulm
OpenGL framework for the VISCOM group at ulm university

Dependencies:
- CMake 3.4 (will not work with older versions, as CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS is supported only since 3.4)
- CUDA 7.0
- OpenGL 4.4
- Boost (http://www.boost.org/)

Dependencies included as submodules:
- assimp (https://github.com/assimp/assimp)
- glad (https://github.com/Dav1dde/glad/tree/c)
- GLM (http://glm.g-truc.net/)
- ImGUI (https://github.com/ocornut/imgui)
- stb (https://github.com/nothings/stb)

To pull all submodules from remote do

```git submodule update --init
```

or use the `--recursive` when cloning the repository.

Boost and CUDA are detected by the cmake file. Set BOOST_ROOT to make sure Boost is found correctly. Boost libraries are assumed to be in BOOST_ROOT/stage/lib.

Other fonts can be used by generating Bitmap fonts using BMFont by AngelCode (http://www.angelcode.com/products/bmfont/) and converting them to a distance field be using the Distance Field AngelCode Font Converter by bitsquid (http://bitsquid.blogspot.de/2010/04/distance-field-based-rendering-of.html).
