# OGLFramework_uulm
OpenGL framework for the VISCOM group at ulm university

Dependencies:
- CMake 3.4 (will not work with older versions, as CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS is supported only since 3.4)
- CUDA 7.0
- OpenGL 4.4
- GLM (http://glm.g-truc.net/)
- GLEW (http://glew.sourceforge.net/)
- FreeImage (http://freeimage.sourceforge.net/)
- Boost (http://www.boost.org/)

The dependencies are expected to be in the Visual Studio include / library directories.

Other fonts can be used by generating Bitmap fonts using BMFont by AngelCode (http://www.angelcode.com/products/bmfont/) and converting them to a distance field be using the Distance Field AngelCode Font Converter by bitsquid (http://bitsquid.blogspot.de/2010/04/distance-field-based-rendering-of.html).
