/**
 * @file   ArcballRotatedSceneRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.28
 *
 * @brief  Definition of a scene object that is rotated by an arcball.
 */

#ifndef ARCBALLROTATEDSCENERENDERABLE_H
#define ARCBALLROTATEDSCENERENDERABLE_H

#include "SceneRenderable.h"
#include "core/Arcball.h"

namespace cgu {

    class ArcballRotatedSceneRenderable : public SceneRenderable
    {
    public:
        ArcballRotatedSceneRenderable(Renderable* renderable, const glm::vec3& pos);

        bool HandleMouse(unsigned int buttonAction, BaseGLWindow* sender);
        void Update(const CameraView& camera);

    protected:
        /** Constructor used for derivations that create their own renderables. */
        ArcballRotatedSceneRenderable(const glm::vec3& pos);

    private:
        /** Holds the arcball for rotation. */
        Arcball rotArcball;
    };
}

#endif /* ARCBALLROTATEDSCENERENDERABLE_H */
