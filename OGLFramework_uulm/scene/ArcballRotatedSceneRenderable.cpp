/**
 * @file   ArcballRotatedSceneRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.28
 *
 * @brief  Implementation of the arcball rotated scene object.
 */

#include "ArcballRotatedSceneRenderable.h"
#include "gfx/CameraView.h"

namespace cgu {

    ArcballRotatedSceneRenderable::ArcballRotatedSceneRenderable(MeshRenderable* renderable, const glm::vec3& pos) :
        SceneRenderable(renderable, pos, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        rotArcball(RI_MOUSE_RIGHT_BUTTON_DOWN, MB_RGHT)
    {

    }

    ArcballRotatedSceneRenderable::ArcballRotatedSceneRenderable(const glm::vec3& pos) :
        SceneRenderable(pos, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        rotArcball(RI_MOUSE_RIGHT_BUTTON_DOWN, MB_RGHT)
    {

    }

    ArcballRotatedSceneRenderable::~ArcballRotatedSceneRenderable()
    {
    }

    bool ArcballRotatedSceneRenderable::HandleMouse(unsigned int buttonAction, float mouseWheelDelta, BaseGLWindow* sender)
    {
        return rotArcball.HandleMouse(buttonAction, sender);
    }

    void ArcballRotatedSceneRenderable::Update(const CameraView& camera, float time, float elapsed)
    {
        glm::quat orient = glm::inverse(rotArcball.GetWorldRotation(camera.GetViewMatrix())) * orientation;
        UpdatePositionOrientation(position, orient);
    }
}
