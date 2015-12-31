/**
 * @file   OrthogonalView.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.22
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
     * @date   2014.02.22
     */
    class OrthogonalView
    {
    public:
        OrthogonalView(float width, float height, ShaderBufferBindingPoints* uniformBindingPoints);
        OrthogonalView(const OrthogonalView&);
        OrthogonalView& operator=(const OrthogonalView&);
        OrthogonalView(OrthogonalView&&);
        OrthogonalView& operator=(OrthogonalView&&);
        virtual ~OrthogonalView();

        void Resize(float width, float height);
        void SetView() const;

    private:
        /** Holds the ortho ubo content. */
        OrthoProjectionBuffer orthoBuffer;
        /** Holds the orthographic projection uniform buffer. */
        std::unique_ptr<GLUniformBuffer> orthoUBO;

    };
}

#endif /* ORTHOGONALVIEW_H */
