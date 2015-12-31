/**
 * @file   Arcball.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.28
 *
 * @brief  Implementation of the arcball.
 */

#include "Arcball.h"
#include "app/BaseGLWindow.h"
#include <glm/gtc/quaternion.hpp>

#undef min
#undef max

namespace cgu {

    Arcball::Arcball(unsigned int theButtonDownFlag, unsigned int theButtonFlag) :
        buttonDownFlag(theButtonDownFlag),
        buttonFlag(theButtonFlag),
        arcballOn(false),
        currentScreenNDC(0.0f),
        lastScreenNDC(0.0f)
    {

    }

    bool Arcball::HandleMouse(unsigned int buttonAction, BaseGLWindow* sender)
    {
        auto handled = false;
        if (buttonAction & buttonDownFlag) {
            arcballOn = true;
            lastScreenNDC = currentScreenNDC = sender->GetMouseAbsoluteNDC();
            handled = true;
        } else if (sender->GetMouseButtonState(buttonFlag)) {
            currentScreenNDC = sender->GetMouseAbsoluteNDC();
            handled = true;
        } else if (!sender->GetMouseButtonState(buttonFlag)) {
            handled = arcballOn;
            arcballOn = false;
        }

        return handled;
    }


    glm::quat Arcball::GetWorldRotation(const glm::mat4& view)
    {
        glm::quat result(1.0f, 0.0f, 0.0f, 0.0f);
        if (currentScreenNDC != lastScreenNDC) {
            auto angle = acos(glm::min(1.0f, glm::dot(lastScreenNDC, currentScreenNDC)));
            auto camAxis = glm::cross(lastScreenNDC, currentScreenNDC);
            auto worldAxis = glm::normalize(glm::vec3(glm::inverse(glm::mat3(view)) * camAxis));
            result = glm::angleAxis(-1.5f * angle, worldAxis);
            lastScreenNDC = currentScreenNDC;
        }
        return result;
    }
}
