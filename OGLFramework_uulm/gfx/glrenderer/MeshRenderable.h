/**
 * @file   MeshRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Januar 2014
 *
 * @brief  Contains the definition of MeshRenderable.
 */

#ifndef MESHRENDERABLE_H
#define MESHRENDERABLE_H

#include "Renderable.h"
#include "gfx/Mesh.h"
#include "main.h"
#include "GPUProgram.h"
#include "GLVertexAttributeArray.h"

namespace cgu {

    /**
     * @brief  Renderable implementation for triangle meshes.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   19. Januar 2014
     */
    class MeshRenderable : public Renderable
    {
    private:
        /** Deleted copy constructor. */
        MeshRenderable(const MeshRenderable&) {};
        /** Deleted copy assignment operator. */
        MeshRenderable& operator=(const MeshRenderable&) {};

    public:
        MeshRenderable(const Mesh& renderMesh);
        virtual ~MeshRenderable();
        MeshRenderable(MeshRenderable&& orig);
        MeshRenderable& operator=(MeshRenderable&& orig);

        virtual void Draw(const VertexAttributeBindings& bindings) const override;
        virtual void FillVertexAttributeBindings(GPUProgram& program,
            VertexAttributeBindings& bindings) const override;

    private:
        /** Holds the mesh to render. */
        const Mesh* mesh;
        /** Holds the vertex buffer object name. */
        GLuint vBuffer;
        /** Holds the index buffer object name of the mesh base. */
        GLuint iBuffer;
        /** Holds the index buffer object names. */
        std::vector<GLuint> iBuffers;

        static void FillIndexBuffer(GLuint iBuffer, const SubMesh* subMesh);
        static void GenerateVertexAttribute(GLVertexAttributeArray* vao, const SubMesh* subMesh,
            const std::vector<BindingLocation>& shaderPositions);
        static void DrawSubMesh(const GLVertexAttributeArray* vao, const SubMesh* subMesh);
    };
}

#endif /* MESHRENDERABLE_H */
