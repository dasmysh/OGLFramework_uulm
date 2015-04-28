/**
 * @file   BaseGLWindow.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Dezember 2013
 *
 * @brief  Constains Implementation for the BaseGLWindow.
 */

#include "main.h"

BaseGLWindow::BaseGLWindow(unsigned int w, unsigned int h) :
GLRenderTarget(w, h),
texManager(nullptr),
matManager(nullptr),
shaderManager(nullptr),
programManager(nullptr),
uboBindingPoints(nullptr)
{
}

/** Returns the texture manager. */
TextureManager* BaseGLWindow::GetTextureManager()
{
    return texManager;
}

/** Returns the material lib manager. */
MaterialLibManager* BaseGLWindow::GetMaterialLibManager()
{
    return matManager;
}

/** Returns the shader manager. */
ShaderManager* BaseGLWindow::GetShaderManager()
{
    return shaderManager;
}

/** Returns the gpu program manager. */
GPUProgramManager* BaseGLWindow::GetGPUProgramaManager()
{
    return programManager;
}

/** Returns the UBO binding points. */
ShaderBufferBindingPoints* BaseGLWindow::GetUBOBindingPoints()
{
    return uboBindingPoints;
}
