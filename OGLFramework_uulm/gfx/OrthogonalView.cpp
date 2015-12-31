/**
 * @file   OrthogonalView.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.22
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

    /** Copy constructor. */
    OrthogonalView::OrthogonalView(const OrthogonalView& rhs) :
        orthoBuffer(rhs.orthoBuffer),
        orthoUBO(new GLUniformBuffer(*rhs.orthoUBO))
    {
        orthoUBO->UploadData(0, sizeof(OrthoProjectionBuffer), &orthoBuffer);
    }

    /** Copy assignment operator. */
    OrthogonalView& OrthogonalView::operator=(const OrthogonalView& rhs)
    {
        OrthogonalView tmp{ rhs };
        std::swap(orthoBuffer, tmp.orthoBuffer);
        std::swap(orthoUBO, tmp.orthoUBO);
        return *this;
    }

    /** Move constructor. */
    OrthogonalView::OrthogonalView(OrthogonalView&& rhs) :
        orthoBuffer(std::move(rhs.orthoBuffer)),
        orthoUBO(std::move(rhs.orthoUBO))
    {
    }

    /** Move assignment operator. */
    OrthogonalView& OrthogonalView::operator=(OrthogonalView&& rhs)
    {
        this->~OrthogonalView();
        orthoBuffer = std::move(rhs.orthoBuffer);
        orthoUBO = std::move(rhs.orthoUBO);
        return *this;
    }

    /** Default destructor. */
    OrthogonalView::~OrthogonalView() = default;

    void OrthogonalView::Resize(float width, float height)
    {
        auto bottom = height;
        auto right = width;
        orthoBuffer.orthoMatrix = glm::ortho(0.0f, right, bottom, 0.0f, 1.0f, -1.0f);
        orthoUBO->UploadData(0, sizeof(OrthoProjectionBuffer), &orthoBuffer);
    }

    void OrthogonalView::SetView() const
    {
        orthoUBO->BindBuffer();
    }
}
