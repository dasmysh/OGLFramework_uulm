/**
 * @file   GLRenderTarget.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the implementation of GLRenderTarget.
 */

#include "GLRenderTarget.h"
#include "../../main.h"
#include "FrameBuffer.h"

namespace cgu {

    /**
     * Constructor.
     */
    GLRenderTarget::GLRenderTarget(unsigned int w, unsigned int h) :
        batchRT(*this),
        fbo(),
        width(w),
        height(h)
    {
    }

    GLRenderTarget::~GLRenderTarget()
    {
    }

    /** Implicit convertion to a GLBatchRenderTarget for direct rendering without the lambda expression in BatchDraw. */
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
}
