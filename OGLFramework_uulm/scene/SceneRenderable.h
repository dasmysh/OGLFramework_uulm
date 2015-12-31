/**
 * @file   SceneRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.28
 *
 * @brief  Definition of a renderable object inside a scene.
 */

#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cgu {

    class CameraView;
    class GPUProgram;
    class MeshRenderable;

    class SceneRenderable
    {
    public:
        SceneRenderable(MeshRenderable* renderable, const glm::vec3& pos, const glm::quat& orient);

        void UpdatePositionOrientation(const glm::vec3& pos, const glm::quat& orient);
        void Draw(const CameraView& camera) const;

    protected:
        /** Constructor used for derivations that create their own renderables. */
        SceneRenderable(const glm::vec3& pos, const glm::quat& orient);

        /** Holds the renderable used for display. */
        MeshRenderable* renderable;
        /** Holds the objects position in space. */
        glm::vec3 position;
        /** Holds the objects orientation. */
        glm::quat orientation;
        /** Holds the objects world matrix. */
        glm::mat4 worldMatrix;
    };
}

#endif /* SCENEOBJECT_H */
