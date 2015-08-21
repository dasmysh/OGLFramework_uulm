/**
 * @file   GUIRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   25. Februar 2014
 *
 * @brief  Contains the implementation of GUIRenderable.
 */

#include "GUIRenderable.h"
#include "ScreenText.h"
#include "GPUProgram.h"

#include <boost/assign.hpp>

namespace cgu {
    GUIRenderable::GUIRenderable(const std::vector<GUIVertex>& vertexData, const std::vector<unsigned int>& indexData,
        GPUProgram* prog) :
        vBuffer(0),
        iBuffer(0),
        indexCount(static_cast<unsigned int>(indexData.size())),
        program(prog),
        attribBind(nullptr)
    {
        CreateVBO(vertexData);
        CreateIBO(indexData);
        CreateVAO();
    }

    GUIRenderable::GUIRenderable(const std::vector<GUIVertex>& vertexData, GPUProgram* prog) :
        vBuffer(0),
        iBuffer(0),
        indexCount(static_cast<unsigned int>(vertexData.size())),
        program(prog),
        attribBind(nullptr)
    {
        CreateVBO(vertexData);
        CreateVAO();
    }

    GUIRenderable::~GUIRenderable()
    {
        if (vBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &vBuffer);
        }

        if (iBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &iBuffer);
        }
    }

    GUIRenderable::GUIRenderable(GUIRenderable&& orig) :
        vBuffer(orig.vBuffer),
        iBuffer(orig.iBuffer),
        indexCount(orig.indexCount),
        program(orig.program),
        attribBind(std::move(orig.attribBind))
    {
        orig.vBuffer = 0;
        orig.iBuffer = 0;
    }

    GUIRenderable& GUIRenderable::operator =(GUIRenderable&& orig)
    {
        if (this != &orig) {
            OGL_CALL(glDeleteBuffers, 1, &vBuffer);
            OGL_CALL(glDeleteBuffers, 1, &iBuffer);
            vBuffer = orig.vBuffer;
            iBuffer = orig.iBuffer;
            indexCount = orig.indexCount;
            program = orig.program;
            attribBind = std::move(orig.attribBind);
            orig.vBuffer = 0;
            orig.iBuffer = 0;
        }
        return *this;
    }

    void GUIRenderable::RenderGeometry()
    {
        attribBind->EnableVertexAttributeArray();
        if (iBuffer != 0) {
            OGL_CALL(glDrawElements, GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        } else {
            OGL_CALL(glDrawArrays, GL_TRIANGLES, 0, indexCount);
        }
        attribBind->DisableVertexAttributeArray();
    }

    void GUIRenderable::CreateVBO(const std::vector<GUIVertex>& vertexData)
    {
        OGL_CALL(glGenBuffers, 1, &vBuffer);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, vertexData.size() * sizeof(GUIVertex),
            vertexData.data(), GL_STATIC_DRAW);
    }

    void GUIRenderable::CreateIBO(const std::vector<unsigned int>& indexData)
    {
        OGL_CALL(glGenBuffers, 1, &iBuffer);
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, iBuffer);
        OGL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int),
            indexData.data(), GL_STATIC_DRAW);
    }

    void GUIRenderable::CreateVAO()
    {
        std::vector<BindingLocation> loc = program->GetAttributeLocations(
            boost::assign::list_of<std::string>("position")("texCoords"));
        attribBind = program->CreateVertexAttributeArray(vBuffer, iBuffer);
        attribBind->StartAttributeSetup();
        attribBind->AddVertexAttribute(loc[0], 3, GL_FLOAT, GL_FALSE, sizeof(GUIVertex), 0);
        attribBind->AddVertexAttribute(loc[1], 2, GL_FLOAT, GL_FALSE, sizeof(GUIVertex), sizeof(glm::vec3));
        attribBind->EndAttributeSetup();
    }
}
