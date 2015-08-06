/**
 * @file   SimpleQuadRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   28. Februar 2014
 *
 * @brief  Contains the implementation of SimpleQuadRenderable.
 */

#include "SimpleQuadRenderable.h"
#include "gfx/glrenderer/GLVertexAttributeArray.h"
#include "gfx/glrenderer/GPUProgram.h"

#include <boost/assign.hpp>

namespace cgu {

    SimpleQuadRenderable::SimpleQuadRenderable(const glm::vec2* vertexData, GPUProgram* prog) :
        vBuffer(0),
        iBuffer(0),
        program(prog),
        attribBind(nullptr)
    {
        OGL_CALL(glGenBuffers, 1, &vBuffer);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertexData, GL_STATIC_DRAW);

        unsigned int indexData[6] = { 0, 1, 2, 2, 1, 3 };
        OGL_CALL(glGenBuffers, 1, &iBuffer);
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, iBuffer);
        OGL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

        std::vector<BindingLocation> loc = program->GetAttributeLocations(
            boost::assign::list_of<std::string>("position"));
        attribBind.reset(new GLVertexAttributeArray(vBuffer, iBuffer));
        attribBind->StartAttributeSetup();
        attribBind->AddVertexAttribute(loc[0], 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
        attribBind->EndAttributeSetup();

    }

    SimpleQuadRenderable::~SimpleQuadRenderable()
    {
        if (vBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &vBuffer);
        }

        if (iBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &iBuffer);
        }
    }

    SimpleQuadRenderable::SimpleQuadRenderable(SimpleQuadRenderable&& orig) :
        vBuffer(orig.vBuffer),
        iBuffer(orig.iBuffer),
        program(orig.program),
        attribBind(std::move(orig.attribBind))
    {
        orig.vBuffer = 0;
        orig.iBuffer = 0;
    }

    SimpleQuadRenderable& SimpleQuadRenderable::operator =(SimpleQuadRenderable&& orig)
    {
        if (this != &orig) {
            OGL_CALL(glDeleteBuffers, 1, &vBuffer);
            OGL_CALL(glDeleteBuffers, 1, &iBuffer);
            vBuffer = orig.vBuffer;
            iBuffer = orig.iBuffer;
            program = orig.program;
            attribBind = std::move(orig.attribBind);
            orig.vBuffer = 0;
            orig.iBuffer = 0;
        }
        return *this;
    }

    void SimpleQuadRenderable::Render()
    {
        attribBind->EnableVertexAttributeArray();
        OGL_CALL(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        attribBind->DisableVertexAttributeArray();
    }
}
