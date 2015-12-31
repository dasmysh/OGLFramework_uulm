/**
 * @file   BaseGLWindow.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2013.12.19
 *
 * @brief  Contains Implementation for the BaseGLWindow.
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

    BaseGLWindow::~BaseGLWindow() = default;

    /** Returns the mouse pointer coordinates in normalized device coordinates. */
    glm::vec3 BaseGLWindow::GetMouseAbsoluteNDC() const
    {
        glm::vec3 result((2.0f * mouseAbsolute.x - width) / width,
            -(2.0f * mouseAbsolute.y - height) / height, 0.0f);
        result = glm::clamp(result, glm::vec3(-1.0f), glm::vec3(1.0f));

        auto length_squared = glm::dot(result, result);
        if (length_squared <= 1.0f)
            result.z = sqrtf(1.0f - length_squared);
        else
            result = glm::normalize(result);

        return result;
    }
}
