/**
 * @file  ScreenQuadRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   28. Februar 2014
 *
 * @brief  Contains the implementation of SimpleQuadRenderable.
 */

#include "ScreenQuadRenderable.h"
#include "gfx/glrenderer/GLVertexAttributeArray.h"
#include "gfx/glrenderer/GPUProgram.h"

#include <boost/assign.hpp>

namespace cgu {

    ScreenQuadRenderable::ScreenQuadRenderable() :
        vBuffer(0)
    {
        std::array<glm::vec2, 4> vertexData;
        OGL_CALL(glGenBuffers, 1, &vBuffer);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertexData.data(), GL_STATIC_DRAW);

        FillAttributeBindings();

    }

    ScreenQuadRenderable::ScreenQuadRenderable(const ScreenQuadRenderable& rhs) :
        ScreenQuadRenderable()
    {
    }

    ScreenQuadRenderable& ScreenQuadRenderable::operator=(ScreenQuadRenderable rhs)
    {
        std::swap(vBuffer, rhs.vBuffer);
        std::swap(vertexAttribs, rhs.vertexAttribs);
        return *this;
    }

    ScreenQuadRenderable::ScreenQuadRenderable(ScreenQuadRenderable&& rhs) :
        vBuffer(rhs.vBuffer),
        vertexAttribs(std::move(rhs.vertexAttribs))
    {
        rhs.vBuffer = 0;
    }

    ScreenQuadRenderable& ScreenQuadRenderable::operator=(ScreenQuadRenderable&& rhs)
    {
        if (this != &rhs) {
            this->~ScreenQuadRenderable();
            vBuffer = rhs.vBuffer;
            vertexAttribs = std::move(rhs.vertexAttribs);
            rhs.vBuffer = 0;
        }
        return *this;
    }

    ScreenQuadRenderable::~ScreenQuadRenderable()
    {
        if (vBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &vBuffer);
        }
    }

    void ScreenQuadRenderable::FillAttributeBindings()
    {
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        vertexAttribs.reset(new GLVertexAttributeArray(vBuffer, 0));

        vertexAttribs->StartAttributeSetup();
        vertexAttribs->EndAttributeSetup();
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    }

    void ScreenQuadRenderable::Draw(GPUProgram* program) const
    {
        vertexAttribs->EnableVertexAttributeArray();
        OGL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
        vertexAttribs->DisableVertexAttributeArray();
    }
}
