/**
 * @file   OrthogonalView.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   22. Februar 2014
 *
 * @brief  Contains the definition of OrthogonalView.
 */

#ifndef ORTHOGONALVIEW_H
#define ORTHOGONALVIEW_H

#include "main.h"

namespace cgu {
    class ShaderBufferBindingPoints;
    class GLUniformBuffer;

    struct OrthoProjectionBuffer
    {
        glm::mat4 orthoMatrix;
    };

    /**
     * @brief  Represents a orthogonal view to be used for rendering.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   22. Februar 2014
     */
    class OrthogonalView
    {
    private:
        /** Deleted copy constructor. */
        OrthogonalView(const OrthogonalView&) = delete;
        /** Deleted copy assignment operator. */
        OrthogonalView& operator=(const OrthogonalView&) = delete;

    public:
        OrthogonalView(float width, float height, ShaderBufferBindingPoints* uniformBindingPoints);
        virtual ~OrthogonalView();

        void Resize(float width, float height);
        void SetView();

    private:
        /** Holds the ortho ubo content. */
        OrthoProjectionBuffer orthoBuffer;
        /** Holds the orthographic projection uniform buffer. */
        std::unique_ptr<GLUniformBuffer> orthoUBO;

    };
}

#endif /* ORTHOGONALVIEW_H */
