/**
 * @file   GLRenderTarget.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.15
 *
 * @brief  Contains the implementation of GLRenderTarget.
 */

#include "GLRenderTarget.h"
#include "FrameBuffer.h"

namespace cgu {

    /**
     * Constructor.
     * @param w the render targets width.
     * @param h the render targets height.
     */
    GLRenderTarget::GLRenderTarget(unsigned int w, unsigned int h) :
        batchRT(*this),
        fbo(),
        width(w),
        height(h)
    {
    }

    /**
     * Creates a new render target with given FrameBufferDescriptor.
     * @param w the render targets width.
     * @param h the render targets height.
     * @param desc the frame buffer description.
     */
    GLRenderTarget::GLRenderTarget(unsigned int w, unsigned int h, const FrameBufferDescriptor& desc) :
        batchRT(*this),
        fbo(w, h, desc),
        width(w),
        height(h)
    {
    }

    GLRenderTarget::~GLRenderTarget()
    {
    }

    /** Implicit conversion to a GLBatchRenderTarget for direct rendering without the lambda expression in BatchDraw. */
    GLRenderTarget::operator GLBatchRenderTarget&()
    {
        return batchRT;
    }

    /**
     * Resizes the render target (ie. sets the new view port).
     * @param w the windows new width
     * @param h the windows new height
     */
    void GLRenderTarget::Resize(unsigned int w, unsigned int h)
    {
        width = w;
        height = h;
        fbo.Resize(width, height);
    }

    /**
     * Makes multiple rendering calls all using this render target. The rendering calls can be defined
     * as a lambda expression or any other functor.
     * @param batch the rendering calls as a functor to be executed while this render target is set
     */
    void GLRenderTarget::BatchDraw(std::function<void(GLBatchRenderTarget&) > batch)
    {
        fbo.UseAsRenderTarget();

        batch(batchRT);
    }

    /**
     * Makes multiple rendering calls all using this render target. The rendering calls can be defined
     * as a lambda expression or any other functor.
     * @param drawBufferIndices the indices in the draw buffer to be used.
     * @param batch the rendering calls as a functor to be executed while this render target is set
     */
    void GLRenderTarget::BatchDraw(const std::vector<unsigned int> drawBufferIndices, std::function<void(GLBatchRenderTarget&) > batch)
    {
        fbo.UseAsRenderTarget(drawBufferIndices);

        batch(batchRT);
    }
}
