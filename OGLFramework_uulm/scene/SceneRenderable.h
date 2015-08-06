/**
 * @file   SceneRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.28
 *
 * @brief  Definition of a renderable object inside a scene.
 */

#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "main.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cgu {

    class CameraView;
    class GPUProgram;
    class Renderable;

    class SceneRenderable
    {
    public:
        SceneRenderable(Renderable* renderable, const glm::vec3& pos, const glm::quat& orient);

        void UpdatePositionOrientation(const glm::vec3& pos, const glm::quat& orient);
        void Draw(const CameraView& camera, GPUProgram* program, VertexAttributeBindings* vaBindings);

    protected:
        /** Constructor used for derivations that create their own renderables. */
        SceneRenderable(const glm::vec3& pos, const glm::quat& orient);

        /** Holds the renderable used for display. */
        Renderable* renderable;
        /** Holds the objects position in space. */
        glm::vec3 position;
        /** Holds the objects orientation. */
        glm::quat orientation;
        /** Holds the objects world matrix. */
        glm::mat4 worldMatrix;
    };
}

#endif /* SCENEOBJECT_H */
