/**
 * @file   OrthogonalView.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   22. Februar 2014
 *
 * @brief  Contains the implementation of OrthogonalView.
 */

#include "OrthogonalView.h"
#include "glrenderer/GLUniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace cgu {
    OrthogonalView::OrthogonalView(float width, float height, ShaderBufferBindingPoints* uniformBindingPoints)
    {
        orthoUBO.reset(new GLUniformBuffer(orthoProjectionUBBName, sizeof(OrthoProjectionBuffer), uniformBindingPoints));
        Resize(width, height);
    }

    OrthogonalView::~OrthogonalView()
    {
    }

    void OrthogonalView::Resize(float width, float height)
    {
        float bottom = height;
        float right = width;
        orthoBuffer.orthoMatrix = glm::ortho(0.0f, right, bottom, 0.0f, 1.0f, -1.0f);
        orthoUBO->UploadData(0, sizeof(OrthoProjectionBuffer), &orthoBuffer);
    }

    void OrthogonalView::SetView()
    {
        orthoUBO->BindBuffer();
    }
}
