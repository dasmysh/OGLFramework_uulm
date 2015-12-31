/**
 * @file   ScreenQuadRenderable.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2015.21.12
 *
 * @brief  Contains the definition of ScreenQuadRenderable.
 */

#ifndef SIMPLEQUADRENDERABLE_H
#define SIMPLEQUADRENDERABLE_H

#include "main.h"

namespace cgu {

    class GPUProgram;

    /**
     * @brief
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2015.21.12
     */
    class ScreenQuadRenderable
    {
    public:
        ScreenQuadRenderable();
        ScreenQuadRenderable(const ScreenQuadRenderable&);
        ScreenQuadRenderable& operator=(ScreenQuadRenderable);
        ScreenQuadRenderable(ScreenQuadRenderable&& orig);
        ScreenQuadRenderable& operator=(ScreenQuadRenderable&& orig);
        ~ScreenQuadRenderable();

        void Draw() const;

    private:
        /** Holds the vertex buffer object to use. */
        GLuint vBuffer;
        /** Holds the vertex attribute bindings. */
        std::unique_ptr<GLVertexAttributeArray> vertexAttribs;

        void FillAttributeBindings();
    };
}

#endif /* SIMPLEQUADRENDERABLE_H */
