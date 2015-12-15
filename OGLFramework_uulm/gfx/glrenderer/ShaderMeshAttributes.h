/**
 * @file   ShaderMeshAttributes.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.12.15
 *
 * @brief  Contains information a mesh needs for rendering.
 */

#ifndef SHADERMESHATTRIBUTES_H
#define SHADERMESHATTRIBUTES_H

#include "main.h"
#include "gfx/glrenderer/GLVertexAttributeArray.h"

namespace cgu {

    class ShaderMeshAttributes
    {
    public:
        ShaderMeshAttributes();
        ~ShaderMeshAttributes();

        /** Returns the vertex attributes. */
        VertexAttributeBindings& GetVertexAttributes() { return vertexAttributes; }
        /** Returns the vertex attributes. */
        const VertexAttributeBindings& GetVertexAttributes() const { return vertexAttributes; }
        /** Returns the uniform ids. */
        std::vector<BindingLocation>& GetUniformIds() { return uniformIds; }
        /** Returns the uniform ids. */
        const std::vector<BindingLocation>& GetUniformIds() const { return uniformIds; }

    private:
        /** Holds the vertex attributes. */
        VertexAttributeBindings vertexAttributes;
        /** Holds the shader uniform ids (for materials). */
        std::vector<BindingLocation> uniformIds;
    };
}


#endif // SHADERMESHATTRIBUTES_H
