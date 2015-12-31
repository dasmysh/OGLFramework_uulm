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

    /**
     *  Constructor.
     *  @param backProg the GPU program for rendering the back faces.
     *  @param drawProg the GPU program for drawing the volume
     */
    VolumeCubeRenderable::VolumeCubeRenderable(GPUProgram* backProg, GPUProgram* drawProg) :
        vBuffer(0),
        iBuffer(0),
        backProgram(backProg),
        backAttribBind(nullptr),
        drawProgram(drawProg),
        drawAttribBind(nullptr)
    {
        CreateVertexIndexBuffers();
    }

    /**
     *  Copy-Constructor.
     *  @param orig the original VolumeCubeRenderable
     */
    VolumeCubeRenderable::VolumeCubeRenderable(const VolumeCubeRenderable& orig) :
        vBuffer(0),
        iBuffer(0),
        backProgram(orig.backProgram),
        backAttribBind(nullptr),
        drawProgram(orig.drawProgram),
        drawAttribBind(nullptr)
    {
        CreateVertexIndexBuffers();
    }

    /**
     *  Move-Constructor.
     *  @param orig the original VolumeCubeRenderable
     */
    VolumeCubeRenderable::VolumeCubeRenderable(VolumeCubeRenderable&& orig) :
        vBuffer(orig.vBuffer),
        iBuffer(orig.iBuffer),
        backProgram(orig.backProgram),
        backAttribBind(std::move(orig.backAttribBind)),
        drawProgram(orig.drawProgram),
        drawAttribBind(std::move(orig.drawAttribBind))

    {
        orig.vBuffer = 0;
        orig.iBuffer = 0;
        orig.backProgram = nullptr;
        orig.drawProgram = nullptr;
    }

    /**
     *  Assignment operator.
     *  @param orig the original VolumeCubeRenderable
     *  @return the newly assigned VolumeCubeRenderable
     */
    VolumeCubeRenderable& VolumeCubeRenderable::operator=(const VolumeCubeRenderable& orig)
    {
        DeleteVertexIndexBuffers();
        backAttribBind = nullptr;
        drawAttribBind = nullptr;
        backProgram = orig.backProgram;
        drawProgram = orig.drawProgram;
        CreateVertexIndexBuffers();
        return *this;
    }

    /**
     *  Move-Assignment operator.
     *  @param orig the original VolumeCubeRenderable
     *  @return the newly assigned VolumeCubeRenderable
     */
    VolumeCubeRenderable& VolumeCubeRenderable::operator=(VolumeCubeRenderable&& orig)
    {
        DeleteVertexIndexBuffers();
        backAttribBind = orig.backAttribBind;
        orig.backAttribBind = nullptr;
        drawAttribBind = orig.drawAttribBind;
        orig.drawAttribBind = nullptr;
        backProgram = orig.backProgram;
        orig.backProgram = nullptr;
        drawProgram = orig.drawProgram;
        orig.drawProgram = nullptr;
        vBuffer = orig.vBuffer;
        orig.vBuffer = 0;
        iBuffer = orig.iBuffer;
        orig.iBuffer = 0;
        return *this;
    }

    /**
     *  Destructor.
     */
    VolumeCubeRenderable::~VolumeCubeRenderable()
    {
        DeleteVertexIndexBuffers();
    }

    /**
     *  Creates the vertex- and index-buffers and their vertex attribute bindings.
     */
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
        FillVertexAttributeBindings(*backProgram, bindings);
        backAttribBind = bindings[0];
        bindings.clear();
        FillVertexAttributeBindings(*drawProgram, bindings);
        drawAttribBind = bindings[0];
    }

    /**
     *  Deletes the vertex- and index-buffers.
     */
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

    /**
     *  Draws the volumes back faces.
     */
    void VolumeCubeRenderable::DrawBack() const
    {
        backProgram->UseProgram();
        Draw(backAttribBind);
    }

    /**
     *  Draws the volume.
     */
    void VolumeCubeRenderable::Draw() const
    {
        drawProgram->UseProgram();
        Draw(drawAttribBind);
    }

    /**
     *  Internal rendering method.
     *  @param attribBinding the vertex attribute bindings to use.
     */
    // ReSharper disable once CppMemberFunctionMayBeStatic
    void VolumeCubeRenderable::Draw(const GLVertexAttributeArray* attribBinding) const
    {
        attribBinding->EnableVertexAttributeArray();
        OGL_CALL(glDrawElements, GL_TRIANGLES, 36, GL_UNSIGNED_INT, static_cast<char*> (nullptr));
        attribBinding->DisableVertexAttributeArray();
    }

    /**
     *  Fills the vertex attribute bindings.
     *  @param prog the program to create bindings for.
     *  @param bindings the created bindings.
     */
    void VolumeCubeRenderable::FillVertexAttributeBindings(GPUProgram& prog, VertexAttributeBindings& bindings) const
    {
        assert(bindings.size() == 0);

        auto loc = prog.GetAttributeLocations(boost::assign::list_of<std::string>("position")("texPosition"));
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        bindings.push_back(prog.CreateVertexAttributeArray(vBuffer, iBuffer));
        bindings[0]->StartAttributeSetup();
        bindings[0]->AddVertexAttribute(loc[0], 4, GL_FLOAT, GL_FALSE, sizeof(VolumeCubeVertex), 0);
        bindings[0]->AddVertexAttribute(loc[1], 3, GL_FLOAT, GL_FALSE, sizeof(VolumeCubeVertex), sizeof(glm::vec4));
        bindings[0]->EndAttributeSetup();
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    }
}
