/**
 * @file   MeshRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2015.12.15
 *
 * @brief  Contains the implementation of MeshRenderable.
 */

#include "MeshRenderable.h"
#include "GPUProgram.h"
#include "GLTexture2D.h"
#include "GLTexture.h"

namespace cgu {

    /**
     * Constructor.
     * @param renderMesh the Mesh to use for rendering.
     * @param prog the program used for rendering.
     */
    MeshRenderable::MeshRenderable(const Mesh* renderMesh, GPUProgram* prog) :
        mesh(renderMesh),
        vBuffer(0),
        iBuffer(0),
        program(prog)
    {
        OGL_CALL(glGenBuffers, 1, &vBuffer);
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, mesh->faceVertices.size() * sizeof(FaceVertex),
            mesh->faceVertices.data(), GL_STATIC_DRAW);

        OGL_CALL(glGenBuffers, 1, &iBuffer);
        FillIndexBuffer(iBuffer, mesh);

        iBuffers.resize(mesh->subMeshes.size(), 0);
        OGL_CALL(glGenBuffers, static_cast<GLsizei>(iBuffers.size()), iBuffers.data());
        for (unsigned int idx = 0; idx < iBuffers.size(); ++idx) {
            FillIndexBuffer(iBuffers[idx], mesh->subMeshes[idx]);
        }

        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);

        FillMeshAttributeBindings();
    }

    /**
     * Destructor.
     */
    MeshRenderable::~MeshRenderable()
    {
        if (vBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &vBuffer);
        }

        if (iBuffer != 0) {
            OGL_CALL(glDeleteBuffers, 1, &iBuffer);
        }

        if (iBuffers.size() > 0) {
            OGL_CALL(glDeleteBuffers, static_cast<GLsizei>(iBuffers.size()), iBuffers.data());
            iBuffers.clear();
        }
        mesh = nullptr;
    }

    /**
     * Copy constructor.
     * @param orig the original object
     */
    MeshRenderable::MeshRenderable(const MeshRenderable& orig) :
        MeshRenderable(orig.mesh, orig.program)
    {
    }

    /**
     * Move constructor.
     * @param orig the original object
     */
    MeshRenderable::MeshRenderable(MeshRenderable&& orig) :
        mesh(orig.mesh),
        vBuffer(orig.vBuffer),
        iBuffer(orig.iBuffer),
        iBuffers(std::move(orig.iBuffers)),
        program(orig.program),
        attribBinds(std::move(attribBinds))
    {
        orig.mesh = nullptr;
        orig.vBuffer = 0;
        orig.iBuffer = 0;
        orig.program = nullptr;
    }

    /**
     * Copy assignment operator.
     * @param orig the original object
     */
    MeshRenderable& MeshRenderable::operator=(MeshRenderable orig)
    {
        std::swap(mesh, orig.mesh);
        std::swap(vBuffer, orig.vBuffer);
        std::swap(iBuffer, orig.iBuffer);
        std::swap(iBuffers, orig.iBuffers);
        std::swap(program, orig.program);
        std::swap(attribBinds, orig.attribBinds);
        return *this;
    }

    /**
     * Move assignment operator.
     * @param orig the original object
     */
    MeshRenderable& MeshRenderable::operator=(MeshRenderable&& orig)
    {
        if (this != &orig) {
            this->~MeshRenderable();
            mesh = orig.mesh;
            vBuffer = orig.vBuffer;
            iBuffer = orig.iBuffer;
            iBuffers = std::move(orig.iBuffers);
            program = orig.program;
            attribBinds = std::move(orig.attribBinds);
            orig.mesh = nullptr;
            orig.vBuffer = 0;
            orig.iBuffer = 0;
            orig.program = nullptr;
        }
        return *this;
    }

    void MeshRenderable::FillMeshAttributeBindings()
    {
        assert(attribBinds.GetUniformIds().size() == 0);
        assert(attribBinds.GetVertexAttributes().size() == 0);
        auto shaderPositions = program->GetAttributeLocations({ "pos", "tex", "normal" });

        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        attribBinds.GetVertexAttributes().push_back(program->CreateVertexAttributeArray(vBuffer, iBuffer));
        GenerateVertexAttribute(attribBinds.GetVertexAttributes().back(), mesh, shaderPositions);
        for (unsigned int idx = 0; idx < mesh->subMeshes.size(); ++idx) {
            attribBinds.GetVertexAttributes().push_back(program->CreateVertexAttributeArray(vBuffer, iBuffers[idx]));
            GenerateVertexAttribute(attribBinds.GetVertexAttributes().back(), mesh->subMeshes[idx], shaderPositions);
        }
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);

        attribBinds.GetUniformIds() = program->GetUniformLocations({ "diffuseTex", "bumpTex", "bumpMultiplier" });
    }

    void MeshRenderable::Draw() const
    {
        program->UseProgram();
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
        DrawSubMesh(attribBinds.GetVertexAttributes()[0], mesh);
        for (unsigned int idx = 0; idx < iBuffers.size(); ++idx) {
            DrawSubMesh(attribBinds.GetVertexAttributes()[idx + 1], mesh->subMeshes[idx]);
        }
        OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    }

    void MeshRenderable::FillIndexBuffer(GLuint iBuffer, const SubMesh* subMesh)
    {
        OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, iBuffer);
        OGL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, subMesh->faceIndices.size() * sizeof(unsigned int),
            subMesh->faceIndices.data(), GL_STATIC_DRAW);
    }

    void MeshRenderable::GenerateVertexAttribute(GLVertexAttributeArray* vao, const SubMesh* subMesh,
        const std::vector<BindingLocation>& shaderPositions)
    {
        vao->StartAttributeSetup();
        // pos
        if (shaderPositions[0]->iBinding >= 0) {
            vao->AddVertexAttribute(shaderPositions[0], 3, GL_FLOAT, GL_FALSE, sizeof(FaceVertex), 0);
        }

        if (subMesh->faceHasTexture && shaderPositions[1]->iBinding >= 0) {
            vao->AddVertexAttribute(shaderPositions[1], 2, GL_FLOAT, GL_FALSE,
                sizeof(FaceVertex), sizeof(glm::vec3));
        }
        if (subMesh->faceHasNormal && shaderPositions[2]->iBinding >= 0) {
            vao->AddVertexAttribute(shaderPositions[2], 3, GL_FLOAT, GL_FALSE,
                sizeof(FaceVertex), sizeof(glm::vec3) + sizeof(glm::vec2));
        }
        vao->EndAttributeSetup();
    }

    void MeshRenderable::DrawSubMesh(const GLVertexAttributeArray* vao, const SubMesh* subMesh) const
    {
        vao->EnableVertexAttributeArray();
        for (const auto& mtlChunk : subMesh->mtlChunks) {
            if (mtlChunk.material->diffuseTex && attribBinds.GetUniformIds().size() != 0) {
                mtlChunk.material->diffuseTex->GetTexture()->ActivateTexture(GL_TEXTURE0);
                program->SetUniform(attribBinds.GetUniformIds()[0], 0);
            }
            if (mtlChunk.material->bumpTex && attribBinds.GetUniformIds().size() >= 2) {
                mtlChunk.material->bumpTex->GetTexture()->ActivateTexture(GL_TEXTURE1);
                program->SetUniform(attribBinds.GetUniformIds()[1], 1);
                program->SetUniform(attribBinds.GetUniformIds()[2], mtlChunk.material->bumpMultiplier);
            }

            // TODO: set material ...
            GLsizei count = mtlChunk.face_seq_num;
            OGL_CALL(glDrawElements, GL_TRIANGLES, count, GL_UNSIGNED_INT,
                (static_cast<char*> (nullptr)) + (mtlChunk.face_seq_begin * sizeof(unsigned int)));
        }
        vao->DisableVertexAttributeArray();
    }
}
