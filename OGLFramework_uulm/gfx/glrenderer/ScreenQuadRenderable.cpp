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

    ScreenQuadRenderable::ScreenQuadRenderable(GPUProgram* prog) :
        vBuffer(0),
        program(prog)
    {
        std::array<glm::vec2, 4> vertexData;
        OGL_CALL(glGenBuffers, 1, &vBuffer);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertexData.data(), GL_STATIC_DRAW);

        FillAttributeBindings();

    }

    ScreenQuadRenderable::ScreenQuadRenderable(const ScreenQuadRenderable& rhs) :
        ScreenQuadRenderable(rhs.program)
    {
    }

    ScreenQuadRenderable& ScreenQuadRenderable::operator=(ScreenQuadRenderable rhs)
    {
        std::swap(vBuffer, rhs.vBuffer);
        std::swap(program, rhs.program);
        attribBinds = rhs.attribBinds;
        return *this;
    }

    ScreenQuadRenderable::ScreenQuadRenderable(ScreenQuadRenderable&& rhs) :
        vBuffer(rhs.vBuffer),
        attribBinds(std::move(rhs.attribBinds))
    {
        rhs.vBuffer = 0;
    }

    ScreenQuadRenderable& ScreenQuadRenderable::operator=(ScreenQuadRenderable&& rhs)
    {
        if (this != &rhs) {
            this->~ScreenQuadRenderable();
            vBuffer = rhs.vBuffer;
            program = rhs.program;
            attribBinds = std::move(rhs.attribBinds);
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
        assert(attribBinds.GetUniformIds().size() == 0);
        assert(attribBinds.GetVertexAttributes().size() == 0);
        std::vector<BindingLocation> shaderPositions = program->GetAttributeLocations({ "pos" });

        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        attribBinds.GetVertexAttributes().push_back(program->CreateVertexAttributeArray(vBuffer, 0));

        attribBinds.GetVertexAttributes().back()->StartAttributeSetup();
        // pos
        if (shaderPositions[0]->iBinding >= 0) {
            attribBinds.GetVertexAttributes().back()->AddVertexAttribute(shaderPositions[0], 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
        }
        attribBinds.GetVertexAttributes().back()->EndAttributeSetup();
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    }

    void ScreenQuadRenderable::Draw() const
    {
        attribBinds.GetVertexAttributes()[0]->EnableVertexAttributeArray();
        OGL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
        attribBinds.GetVertexAttributes()[0]->DisableVertexAttributeArray();
    }
}
