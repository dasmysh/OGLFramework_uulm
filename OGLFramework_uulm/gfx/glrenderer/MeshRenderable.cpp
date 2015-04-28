/**
 * @file   MeshRenderable.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Januar 2014
 *
 * @brief  Contains the implementation of MeshRenderable.
 */

#include "MeshRenderable.h"

#include <boost/assign.hpp>

#include "GPUProgram.h"

/**
 * Constructor.
 * @param renderMesh the Mesh to use for rendering.
 */
MeshRenderable::MeshRenderable(const Mesh& renderMesh) :
mesh(&renderMesh),
vBuffer(0),
iBuffer(0)
{
    OGL_CALL(glGenBuffers, 1, &vBuffer);
    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
    OGL_CALL(glBufferData, GL_ARRAY_BUFFER, mesh->faceVertices.size() * sizeof (FaceVertex),
            mesh->faceVertices.data(), GL_STATIC_DRAW);

    OGL_CALL(glGenBuffers, 1, &iBuffer);
    FillIndexBuffer(iBuffer, mesh);

    iBuffers.resize(mesh->subMeshes.size(), 0);
    OGL_CALL(glGenBuffers, static_cast<GLsizei>(iBuffers.size()), iBuffers.data());
    for (unsigned int idx = 0; idx < iBuffers.size(); ++idx) {
        FillIndexBuffer(iBuffers[idx], mesh->subMeshes[idx]);
    }

    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
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
 * Move constructor.
 * @param orig the original object
 */
MeshRenderable::MeshRenderable(MeshRenderable&& orig) :
Renderable(std::move(orig)),
mesh(orig.mesh),
vBuffer(orig.vBuffer),
iBuffer(orig.iBuffer),
iBuffers(std::move(orig.iBuffers))
{
    orig.mesh = nullptr;
    orig.vBuffer = 0;
    orig.iBuffer = 0;
}

/**
 * Move assignment operator.
 * @param orig the original object
 */
MeshRenderable& MeshRenderable::operator =(MeshRenderable&& orig)
{
    if (this != &orig) {
        OGL_CALL(glDeleteBuffers, 1, &vBuffer);
        OGL_CALL(glDeleteBuffers, static_cast<GLsizei>(iBuffers.size()), &iBuffers[0]);
        Renderable* tRes = this;
        *tRes = static_cast<Renderable&&> (std::move(orig));
        mesh = orig.mesh;
        vBuffer = orig.vBuffer;
        iBuffer = orig.iBuffer;
        iBuffers = std::move(orig.iBuffers);
        orig.mesh = nullptr;
        orig.vBuffer = 0;
        orig.iBuffer = 0;
    }
    return *this;
}

void MeshRenderable::FillVertexAttributeBindings(GPUProgram& program,
        VertexAttributeBindings& bindings) const
{
    assert(bindings.size() == 0);
    std::vector<BindingLocation> shaderPositions =
        program.GetAttributeLocations(boost::assign::list_of<std::string>("pos")("tex")("normal"));

    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
    bindings.push_back(program.CreateVertexAttributeArray(vBuffer, iBuffer));
    GenerateVertexAttribute(bindings.back(), mesh, shaderPositions);
    for (unsigned int idx = 0; idx < mesh->subMeshes.size(); ++idx) {
        bindings.push_back(program.CreateVertexAttributeArray(vBuffer, iBuffers[idx]));
        GenerateVertexAttribute(bindings.back(), mesh->subMeshes[idx], shaderPositions);
    }
    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

void MeshRenderable::Draw(const VertexAttributeBindings& bindings) const
{
    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vBuffer);
    DrawSubMesh(bindings[0], mesh);
    for (unsigned int idx = 0; idx < iBuffers.size(); ++idx) {
        DrawSubMesh(bindings[idx + 1], mesh->subMeshes[idx]);
    }
    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

void MeshRenderable::FillIndexBuffer(GLuint iBuffer, const SubMesh* subMesh)
{
    OGL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, iBuffer);
    OGL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, subMesh->faceIndices.size() * sizeof (unsigned int),
            subMesh->faceIndices.data(), GL_STATIC_DRAW);
}

void MeshRenderable::GenerateVertexAttribute(GLVertexAttributeArray* vao, const SubMesh* subMesh,
        const std::vector<BindingLocation>& shaderPositions)
{
    vao->StartAttributeSetup();
    // pos
    if (shaderPositions[0]->iBinding >= 0) {
        vao->AddVertexAttribute(shaderPositions[0], 3, GL_FLOAT, GL_FALSE, sizeof (FaceVertex), 0);
    }

    if (subMesh->faceHasTexture && shaderPositions[1]->iBinding >= 0) {
        vao->AddVertexAttribute(shaderPositions[1], 2, GL_FLOAT, GL_FALSE,
                sizeof (FaceVertex), sizeof (glm::vec3));
    }
    if (subMesh->faceHasNormal && shaderPositions[2]->iBinding >= 0) {
        vao->AddVertexAttribute(shaderPositions[2], 3, GL_FLOAT, GL_FALSE,
                sizeof (FaceVertex), sizeof (glm::vec3) + sizeof (glm::vec2));
    }
    vao->EndAttributeSetup();
}

void MeshRenderable::DrawSubMesh(const GLVertexAttributeArray* vao, const SubMesh* subMesh)
{
    vao->EnableVertexAttributeArray();
    for (const SubMeshMaterialChunk& mtlChunk : subMesh->mtlChunks) {
        // TODO: set material ...
        GLsizei count = mtlChunk.face_seq_num;
        OGL_CALL(glDrawElements, GL_TRIANGLES, count, GL_UNSIGNED_INT,
                (static_cast<char*> (nullptr)) + (mtlChunk.face_seq_begin * sizeof (unsigned int)));
    }
    vao->DisableVertexAttributeArray();
}
