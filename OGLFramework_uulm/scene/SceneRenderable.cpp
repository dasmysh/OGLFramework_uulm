/**
 * @file   SceneRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.05.28
 *
 * @brief  Implementation of the scene renderable object.
 */

#include "SceneRenderable.h"
#include "gfx/CameraView.h"
#include "gfx/glrenderer/GPUProgram.h"
#include "gfx/glrenderer/Renderable.h"
#include <glm/glm.hpp>

namespace cgu {

    SceneRenderable::SceneRenderable(Renderable* theRenderable, const glm::vec3& pos, const glm::quat& orient) :
        renderable(theRenderable),
        position(pos),
        orientation(orient),
        worldMatrix(1.0f)
    {
        UpdatePositionOrientation(position, orientation);
    }

    SceneRenderable::SceneRenderable(const glm::vec3& pos, const glm::quat& orient) :
        renderable(nullptr),
        position(pos),
        orientation(orient),
        worldMatrix(1.0f)
    {
        UpdatePositionOrientation(position, orientation);
    }

    void SceneRenderable::UpdatePositionOrientation(const glm::vec3& pos, const glm::quat& orient)
    {
        position = pos;
        orientation = orient;
        glm::mat4 matOrient(glm::mat3_cast(orientation));
        worldMatrix = glm::mat4(matOrient[0], matOrient[1], matOrient[2], glm::vec4(position, 1));
    }

    void SceneRenderable::Draw(const CameraView& camera, GPUProgram* program, VertexAttributeBindings* vaBindings)
    {
        camera.SetView(worldMatrix);
        program->UseProgram();
        renderable->Draw(*vaBindings);
    }
}
