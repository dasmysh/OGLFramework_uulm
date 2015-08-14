/**
 * @file   BaseGLWindow.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Dezember 2013
 *
 * @brief  Constains Implementation for the BaseGLWindow.
 */

#include "main.h"
#include "BaseGLWindow.h"

namespace cgu {

    BaseGLWindow::BaseGLWindow(unsigned int w, unsigned int h) :
        GLRenderTarget(w, h),
        texManager(nullptr),
        matManager(nullptr),
        shaderManager(nullptr),
        programManager(nullptr),
        uboBindingPoints(nullptr),
        mouseButtonState(0),
        mouseRelative(0.0f, 0.0f),
        mouseAbsolute(0.0f, 0.0f),
        keyboardModState(0),
        hadPositionUpdate(false)
    {
    }

    BaseGLWindow::~BaseGLWindow()
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


    /** Returns the mouse pointer coordinates in normalized device coordinates. */
    glm::vec3 BaseGLWindow::GetMouseAbsoluteNDC() const
    {
        glm::vec3 result((2.0f * mouseAbsolute.x - width) / width,
            -(2.0f * mouseAbsolute.y - height) / height, 0.0f);
        result = glm::clamp(result, glm::vec3(-1.0f), glm::vec3(1.0f));

        float length_squared = glm::dot(result, result);
        if (length_squared <= 1.0f)
            result.z = sqrtf(1.0f - length_squared);
        else
            result = glm::normalize(result);

        return result;
    }

    glm::vec2 BaseGLWindow::GetMouseAbsoluteVScreen() const
    {
        float ratio = SCREEN_Y / static_cast<float>(height);
        return mouseAbsolute * ratio;
    }

    glm::vec2 BaseGLWindow::ToMouseAbsoluteVScreen(const glm::vec2& coords) const
    {
        float ratio = SCREEN_Y / static_cast<float>(height);
        return coords / ratio;
    }
}