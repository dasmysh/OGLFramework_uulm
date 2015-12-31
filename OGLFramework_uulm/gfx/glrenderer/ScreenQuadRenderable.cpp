/**
 * @file  ScreenQuadRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2015.21.12
 *
 * @brief  Contains the implementation of SimpleQuadRenderable.
 */

#include "ScreenQuadRenderable.h"
#include "gfx/glrenderer/GLVertexAttributeArray.h"
#include <boost/assign.hpp>

namespace cgu {

    /** Default constructor. */
    ScreenQuadRenderable::ScreenQuadRenderable() :
        vBuffer(0)
    {
        std::array<glm::vec2, 4> vertexData;
        OGL_CALL(glGenBuffers, 1, &vBuffer);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertexData.data(), GL_STATIC_DRAW);

        FillAttributeBindings();

    }

    /** Copy constructor. */
    ScreenQuadRenderable::ScreenQuadRenderable(const ScreenQuadRenderable&) :
        ScreenQuadRenderable()
    {
    }

    /** Copy assignment operator. */
    ScreenQuadRenderable& ScreenQuadRenderable::operator=(ScreenQuadRenderable rhs)
    {
        std::swap(vBuffer, rhs.vBuffer);
        std::swap(vertexAttribs, rhs.vertexAttribs);
        return *this;
    }

    /** Move constructor. */
    ScreenQuadRenderable::ScreenQuadRenderable(ScreenQuadRenderable&& rhs) :
        vBuffer(rhs.vBuffer),
        vertexAttribs(std::move(rhs.vertexAttribs))
    {
        rhs.vBuffer = 0;
    }

    /** Move assignment operator. */
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

    void ScreenQuadRenderable::Draw() const
    {
        vertexAttribs->EnableVertexAttributeArray();
        OGL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
        vertexAttribs->DisableVertexAttributeArray();
    }
}
