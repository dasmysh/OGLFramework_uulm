/**
 * @file   GLRenderTarget.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the definition of GLRenderTarget.
 */

#ifndef GLRENDERTARGET_H
#define GLRENDERTARGET_H

#pragma warning(push, 3)
#include <functional>
#pragma warning(pop)

#include "GLBatchRenderTarget.h"
#include "FrameBuffer.h"

namespace cgu {

    /**
     * @brief  Represents an OpenGL render target.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   15. Januar 2014
     */
    class GLRenderTarget
    {
    public:
        GLRenderTarget(unsigned int w, unsigned int h);
        GLRenderTarget(unsigned int w, unsigned int h, const FrameBufferDescriptor& desc);
        virtual ~GLRenderTarget();

        operator GLBatchRenderTarget&();
        virtual void Resize(unsigned int width, unsigned int height);
        void BatchDraw(std::function<void(GLBatchRenderTarget&) > batch);
        void BatchDraw(const std::vector<unsigned int> drawBufferIndices, std::function<void(GLBatchRenderTarget&) > batch);
        const std::vector<std::unique_ptr<GLTexture>>& GetTextures() const { return fbo.GetTextures(); };

    protected:
        /** Holds the internally used GLBatchRenderTarget. */
        GLBatchRenderTarget batchRT;
        /** Holds the render targets OpenGL name. */
        FrameBuffer fbo;
        /** Holds the windows width. */
        unsigned int width;
        /** Holds the windows height. */
        unsigned int height;

    private:
    };
}

#endif /* GLRENDERTARGET_H */
