/**
 * @file   GUIRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   25. Februar 2014
 *
 * @brief  Contains the definition of GUIRenderable.
 */

#ifndef GUIRENDERABLE_H
#define GUIRENDERABLE_H

#include "GLVertexAttributeArray.h"
#include "gfx/Vertices.h"

namespace cgu {

    class GPUProgram;
    class ScreenText;

    /**
     * @brief  Encapsulates rendering for gui objects or other screen-aligned-quad-based objects.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   25. Februar 2014
     */
    class GUIRenderable
    {
    private:
        /** Deleted copy constructor. */
        GUIRenderable(const GUIRenderable&) {};
        /** Deleted copy assignment operator. */
        GUIRenderable& operator=(const GUIRenderable&) { return *this; };

    public:
        GUIRenderable(const std::vector<GUIVertex>& vertexData,
            const std::vector<unsigned int>& indexData, GPUProgram* prog);
        GUIRenderable(const std::vector<GUIVertex>& vertexData, GPUProgram* prog);
        virtual ~GUIRenderable();

        /** Default move constructor. */
        GUIRenderable(GUIRenderable&& orig);
        /** Default move assignment operator. */
        GUIRenderable& operator=(GUIRenderable&& orig);

        void RenderGeometry();

    private:
        /** Holds the vertex buffer object to use. */
        GLuint vBuffer;
        /** Holds the index buffer object name. */
        GLuint iBuffer;
        /** Holds the number of elements to render. */
        unsigned int indexCount;
        /** Holds the rendering gpu program. */
        GPUProgram* program;
        /** Holds the vertex attribute bindings for the shader. */
        std::unique_ptr<GLVertexAttributeArray> attribBind;

        void CreateVBO(const std::vector<GUIVertex>& vertexData);
        void CreateIBO(const std::vector<unsigned int>& indexData);
        void CreateVAO();

    };
}

#endif /* GUIRENDERABLE_H */
