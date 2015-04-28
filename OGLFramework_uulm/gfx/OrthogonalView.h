/**
 * @file   OrthogonalView.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   22. Februar 2014
 *
 * @brief  Contains the definition of OrthogonalView.
 */

#ifndef ORTHOGONALVIEW_H
#define ORTHOGONALVIEW_H

#include "glrenderer/GLUniformBuffer.h"
#include "main.h"

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
    OrthogonalView(const OrthogonalView&) {};
    /** Deleted copy assignment operator. */
    OrthogonalView& operator=(const OrthogonalView&) { return *this; };

public:
    OrthogonalView(float aspectRatio, ShaderBufferBindingPoints* uniformBindingPoints);
    virtual ~OrthogonalView() {};

    void Resize(float aspectRatio);
    void SetView();

private:
    /** Holds the ortho ubo content. */
    OrthoProjectionBuffer orthoBuffer;
    /** Holds the orthographic projection uniform buffer. */
    std::unique_ptr<GLUniformBuffer> orthoUBO;


};

#endif /* ORTHOGONALVIEW_H */
