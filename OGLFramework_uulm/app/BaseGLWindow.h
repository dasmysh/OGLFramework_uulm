/**
 * @file   BaseGLWindow.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2013.12.19
 *
 * @brief  Contains the base class for all OpenGL windows and definitions needed.
 */

#ifndef BASEGLWINDOW_H
#define BASEGLWINDOW_H

#include "gfx/glrenderer/GLRenderTarget.h"
#include "gfx/glrenderer/FrameBuffer.h"

namespace cgu {

    class TextureManager;
    class MaterialLibManager;
    class ShaderManager;
    class GPUProgramManager;
    class ShaderBufferBindingPoints;

    /**
     * @brief The base class for all OpenGL windows.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2013.12.19
     */
    class BaseGLWindow : public GLRenderTarget
    {
    public:
        BaseGLWindow(unsigned int w, unsigned int h);
        virtual ~BaseGLWindow();

        /**
         * Shows a question message box.
         * @param title the message boxes title
         * @param content the message boxes content
         * @return returns <code>true</code> if the user pressed 'yes' <code>false</code> if 'no'
         */
        virtual bool MessageBoxQuestion(const std::string& title, const std::string& content) = 0;
        /** Swaps buffers and shows the content rendered since last call of Present(). */
        virtual void Present() = 0;
        /** Closes the window. */
        virtual void CloseWindow() = 0;

        /** Returns the windows width. */
        unsigned int GetWidth() const
        {
            return width;
        };

        /** Returns the windows height. */
        unsigned int GetHeight() const
        {
            return height;
        };

        unsigned int GetMouseButtonState(unsigned int btn) const { return mouseButtonState & btn; };
        glm::vec2 GetMouseRelative() const { return mouseRelative; };
        glm::vec2 GetMouseAbsolute() const { return mouseAbsolute; };
        glm::vec3 GetMouseAbsoluteNDC() const;
        unsigned int GetKeyboardModState(unsigned int modKey) const { return keyboardModState & modKey; };
        bool HadPositionUpdate() const { return hadPositionUpdate; };
        void HandledPositionUpdate() { hadPositionUpdate = false; };

        /*TextureManager* GetTextureManager() const;
        MaterialLibManager* GetMaterialLibManager() const;
        ShaderManager* GetShaderManager() const;
        GPUProgramManager* GetGPUProgramaManager() const;
        ShaderBufferBindingPoints* GetUBOBindingPoints() const;*/

    protected:
        /** Holds the texture manager. */
        TextureManager* texManager;
        /** Holds the material lib manager. */
        MaterialLibManager* matManager;
        /** Holds the shader manager. */
        ShaderManager* shaderManager;
        /** Holds the GPU program manager. */
        GPUProgramManager* programManager;
        /** Holds the uniform binding points. */
        ShaderBufferBindingPoints* uboBindingPoints;
        /** holds the current mouse button state. */
        unsigned int mouseButtonState;
        /** holds the current relative mouse position. */
        glm::vec2 mouseRelative;
        /** holds the current absolute mouse position. */
        glm::vec2 mouseAbsolute;
        /** holds the state of keyboard modificators (shift, ctrl, alt). */
        unsigned int keyboardModState;
        /** holds whether there was an update to the absolute position. */
        bool hadPositionUpdate;
    };
}
#endif /* BASEGLWINDOW_H */
