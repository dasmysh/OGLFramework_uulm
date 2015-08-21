/**
 * @file   VolumeCubeRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.19
 *
 * @brief  Implementation of a renderable used to determine starting and end positions for raycasting.
 */

#include "VolumeCubeRenderable.h"
#include "gfx/glrenderer/GPUProgram.h"
#include <boost/assign.hpp>

namespace cgu {

    VolumeCubeRenderable::VolumeCubeRenderable(GPUProgram* prog) :
        vBuffer(0),
        iBuffer(0),
        program(prog),
        attribBind(nullptr)
    {
        CreateVertexIndexBuffers();
    }

    VolumeCubeRenderable::VolumeCubeRenderable(const VolumeCubeRenderable& orig) :
        vBuffer(0),
        iBuffer(0),
        program(orig.program),
        attribBind(nullptr)
    {
        CreateVertexIndexBuffers();
    }

    VolumeCubeRenderable::VolumeCubeRenderable(VolumeCubeRenderable&& orig) :
        vBuffer(orig.vBuffer),
        iBuffer(orig.iBuffer),
        program(orig.program),
        attribBind(std::move(orig.attribBind))

    {
        orig.vBuffer = 0;
        orig.iBuffer = 0;
        orig.program = nullptr;
    }

    VolumeCubeRenderable& VolumeCubeRenderable::operator=(const VolumeCubeRenderable& orig)
    {
        DeleteVertexIndexBuffers();
        attribBind = nullptr;
        program = orig.program;
        CreateVertexIndexBuffers();
        return *this;
    }

    VolumeCubeRenderable& VolumeCubeRenderable::operator=(VolumeCubeRenderable&& orig)
    {
        DeleteVertexIndexBuffers();
        attribBind = orig.attribBind;
        orig.attribBind = nullptr;
        program = orig.program;
        orig.program = nullptr;
        vBuffer = orig.vBuffer;
        orig.vBuffer = 0;
        iBuffer = orig.iBuffer;
        orig.iBuffer = 0;
        return *this;
    }

    VolumeCubeRenderable::~VolumeCubeRenderable()
    {
        DeleteVertexIndexBuffers();
    }

    void VolumeCubeRenderable::CreateVertexIndexBuffers()
    {
        std::vector<VolumeCubeVertex> vertices;
        vertices.push_back(VolumeCubeVertex{ glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) });
        vertices.push_back(VolumeCubeVertex{ glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) });
        vertices.push_back(VolumeCubeVertex{ glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        vertices.push_back(VolumeCubeVertex{ glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f) });
        vertices.push_back(VolumeCubeVertex{ glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) });
        vertices.push_back(VolumeCubeVertex{ glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f) });
        vertices.push_back(VolumeCubeVertex{ glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f) });
        vertices.push_back(VolumeCubeVertex{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f) });

        OGL_CALL(glGenBuffers, 1, &vBuffer);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, 8 * sizeof(VolumeCubeVertex), vertices.data(), GL_STATIC_DRAW);

        unsigned int indexData[36] = {
            1, 0, 3, 3, 0, 2,
            4, 5, 6, 6, 5, 7,
            0, 1, 4, 4, 1, 5,
            3, 2, 7, 7, 2, 6,
            1, 3, 5, 5, 3, 7,
            0, 4, 2, 2, 4, 6
        };
        OGL_CALL(glGenBuffers, 1, &iBuffer);
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, iBuffer);
        OGL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

        VertexAttributeBindings bindings;
        FillVertexAttributeBindings(*program, bindings);
        attribBind = bindings[0];
    }

    void VolumeCubeRenderable::DeleteVertexIndexBuffers()
    {
        if (vBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &vBuffer);
            vBuffer = 0;
        }

        if (iBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &iBuffer);
            iBuffer = 0;
        }
    }

    void VolumeCubeRenderable::Draw(const VertexAttributeBindings& bindings) const
    {
        Draw(bindings[0]);
    }

    void VolumeCubeRenderable::Draw() const
    {
        Draw(attribBind);
    }

    void VolumeCubeRenderable::Draw(const GLVertexAttributeArray* attribBinding) const
    {
        program->UseProgram();
        attribBinding->EnableVertexAttributeArray();
        OGL_CALL(glDrawElements, GL_TRIANGLES, 36, GL_UNSIGNED_INT, static_cast<char*> (nullptr));
        attribBinding->DisableVertexAttributeArray();
    }

    void VolumeCubeRenderable::FillVertexAttributeBindings(GPUProgram& prog, VertexAttributeBindings& bindings) const
    {
        assert(bindings.size() == 0);

        std::vector<BindingLocation> loc = prog.GetAttributeLocations(boost::assign::list_of<std::string>("position")("texPosition"));
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        bindings.push_back(prog.CreateVertexAttributeArray(vBuffer, iBuffer));
        bindings[0]->StartAttributeSetup();
        bindings[0]->AddVertexAttribute(loc[0], 4, GL_FLOAT, GL_FALSE, sizeof(VolumeCubeVertex), 0);
        bindings[0]->AddVertexAttribute(loc[1], 3, GL_FLOAT, GL_FALSE, sizeof(VolumeCubeVertex), sizeof(glm::vec4));
        bindings[0]->EndAttributeSetup();
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    }
}
