/**
 * @file   VolumeCubeRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.19
 *
 * @brief  Definition of a renderable used to determine starting and end positions for raycasting.
 */

#ifndef VOLUMECUBERENDERABLE_H
#define VOLUMECUBERENDERABLE_H

#include "gfx\glrenderer\Renderable.h"

namespace cgu {

    struct VolumeCubeVertex
    {
        glm::vec4 pos;
        glm::vec3 posTex;
    };

    class VolumeCubeRenderable : public Renderable
    {
    public:
        VolumeCubeRenderable(GPUProgram* backProg, GPUProgram* drawProg);
        VolumeCubeRenderable(const VolumeCubeRenderable& orig);
        VolumeCubeRenderable(VolumeCubeRenderable&& orig);
        VolumeCubeRenderable& operator=(const VolumeCubeRenderable& orig);
        VolumeCubeRenderable& operator=(VolumeCubeRenderable&& orig);
        ~VolumeCubeRenderable();

        virtual void Draw(const VertexAttributeBindings& bindings) const override;
        void Draw() const;
        void DrawBack() const;
        virtual void FillVertexAttributeBindings(GPUProgram& program, VertexAttributeBindings& bindings) const override;

    private:
        /** Holds the vertex buffer object to use. */
        GLuint vBuffer;
        /** Holds the index buffer object name. */
        GLuint iBuffer;
        /** Holds the rendering GPU program for back faces. */
        GPUProgram* backProgram;
        /** Holds the vertex attribute bindings for the back faces shader. */
        GLVertexAttributeArray* backAttribBind;
        /** Holds the rendering GPU program for drawing. */
        GPUProgram* drawProgram;
        /** Holds the vertex attribute bindings for the draw shader. */
        GLVertexAttributeArray* drawAttribBind;

        void CreateVertexIndexBuffers();
        void DeleteVertexIndexBuffers();
        void Draw(const GLVertexAttributeArray* attribBinding) const;
    };
}

#endif // VOLUMECUBERENDERABLE_H
