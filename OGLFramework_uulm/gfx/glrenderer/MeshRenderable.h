/**
 * @file   MeshRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   19. Januar 2014
 *
 * @brief  Contains the definition of MeshRenderable.
 */

#ifndef MESHRENDERABLE_H
#define MESHRENDERABLE_H

#include "gfx/Mesh.h"
#include "main.h"
#include "GPUProgram.h"
#include "gfx/glrenderer/ShaderMeshAttributes.h"

namespace cgu {

    /**
     * @brief  Renderable implementation for triangle meshes.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2015.12.15
     */
    class MeshRenderable
    {
    public:
        MeshRenderable(const Mesh* renderMesh, GPUProgram* program);
        virtual ~MeshRenderable();
        MeshRenderable(const MeshRenderable&);
        MeshRenderable& operator=(MeshRenderable);
        MeshRenderable(MeshRenderable&&);
        MeshRenderable& operator=(MeshRenderable&&);

        void Draw() const;

    private:
        /** Holds the mesh to render. */
        const Mesh* mesh;
        /** Holds the vertex buffer object name. */
        GLuint vBuffer;
        /** Holds the index buffer object name of the mesh base. */
        GLuint iBuffer;
        /** Holds the index buffer object names. */
        std::vector<GLuint> iBuffers;
        /** Holds the rendering GPU program for drawing. */
        GPUProgram* program;
        /** Holds the shader attribute bindings for the shader. */
        ShaderMeshAttributes attribBinds;

        void FillMeshAttributeBindings();
        static void FillIndexBuffer(GLuint iBuffer, const SubMesh* subMesh);
        static void GenerateVertexAttribute(GLVertexAttributeArray* vao, const SubMesh* subMesh,
            const std::vector<BindingLocation>& shaderPositions);
        void DrawSubMesh(const GLVertexAttributeArray* vao, const SubMesh* subMesh) const;
    };
}

#endif /* MESHRENDERABLE_H */
