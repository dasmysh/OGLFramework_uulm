/**
 * @file   BaseGLWindow.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Dezember 2013
 *
 * @brief  Contains the base class for all OpenGL windows and definitions needed.
 */

#ifndef BASEGLWINDOW_H
#define BASEGLWINDOW_H

#include "gfx/glrenderer/GLRenderTarget.h"
#include "gfx/glrenderer/FrameBuffer.h"

class TextureManager;
class MaterialLibManager;
class ShaderManager;
class GPUProgramManager;
class ShaderBufferBindingPoints;

/**
 * @brief The base class for all OpenGL windows.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Dezember 2013
 */
class BaseGLWindow : public GLRenderTarget
{
public:
    BaseGLWindow(unsigned int w, unsigned int h);
    virtual ~BaseGLWindow() {};

    /**
     * Shows a question message box.
     * @param title the message boxes title
     * @param content the message boxes content
     * @return returns <code>true</code> if the user pressed 'yes' <code>false</code> if 'no'
     */
    virtual bool MessageBoxQuestion(const std::string& title, const std::string content) = 0;
    /** Swaps buffers and shows the content rendered since last call of Present(). */
    virtual void Present() = 0;
    /** Closes the window. */
    virtual void CloseWindow() = 0;

    /** Returns the windows width. */
    unsigned int GetWidth()
    {
        return width;
    };

    /** Returns the windows height. */
    unsigned int GetHeight()
    {
        return height;
    };

    TextureManager* GetTextureManager();
    MaterialLibManager* GetMaterialLibManager();
    ShaderManager* GetShaderManager();
    GPUProgramManager* GetGPUProgramaManager();
    ShaderBufferBindingPoints* GetUBOBindingPoints();

protected:
    /** Holds the texture manager. */
    TextureManager* texManager;
    /** Holds the material lib manager. */
    MaterialLibManager* matManager;
    /** Holds the shader manager. */
    ShaderManager* shaderManager;
    /** Holds the gpu program manager. */
    GPUProgramManager* programManager;
    /** Holds the uniform binding points. */
    ShaderBufferBindingPoints* uboBindingPoints;

private:
};

#endif /* BASEGLWINDOW_H */
