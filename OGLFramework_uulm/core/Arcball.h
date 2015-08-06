/**
 * @file   Arcball.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.28
 *
 * @brief  Definition of the arcball.
 */

#ifndef ARCBALL_H
#define ARCBALL_H

#include <glm/glm.hpp>
namespace cgu {

    class BaseGLWindow;

    /**
    * @brief  Helper class for generic arcballs.
    *
    * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
    * @date   2015.05.28
    */
    class Arcball
    {
    public:
        Arcball(unsigned int buttonDownFlag, unsigned int buttonFlag);

        bool HandleMouse(unsigned int buttonAction, BaseGLWindow* sender);
        glm::quat GetWorldRotation(const glm::mat4& view);

    private:
        /** Holds the action button flag to use. */
        const unsigned int buttonDownFlag;
        /** Holds the button flag to use. */
        const unsigned int buttonFlag;
        /** Holds whether the arcball is currently rotated. */
        bool arcballOn;
        /** holds the current arcball position in normalized device coordinates. */
        glm::vec3 currentScreenNDC;
        /** holds the last arcball position in normalized device coordinates. */
        glm::vec3 lastScreenNDC;
    };
}

#endif /* ARCBALL_H */
