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
#include "gfx/glrenderer/ShaderMeshAttributes.h"

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
        ScreenQuadRenderable(GPUProgram* prog);
        ScreenQuadRenderable(const ScreenQuadRenderable&);
        ScreenQuadRenderable& operator=(ScreenQuadRenderable);
        ScreenQuadRenderable(ScreenQuadRenderable&& orig);
        ScreenQuadRenderable& operator=(ScreenQuadRenderable&& orig);
        ~ScreenQuadRenderable();

        void Draw() const;

    private:
        /** Holds the vertex buffer object to use. */
        GLuint vBuffer;
        /** Holds the rendering GPU program. */
        GPUProgram* program;
        /** Holds the shader attribute bindings for the shader. */
        ShaderMeshAttributes attribBinds;

        void FillAttributeBindings();
    };
}

#endif /* SIMPLEQUADRENDERABLE_H */
