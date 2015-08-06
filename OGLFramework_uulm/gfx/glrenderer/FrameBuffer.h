/**
 * @file   FrameBuffer.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.17
 *
 * @brief  Contains the definition of FrameBuffer.
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
#include <GL/glew.h>
#include "GLTexture.h"

namespace cgu {

    /** Describes a render buffer. */
    struct RenderBufferDescriptor
    {
        /** Holds the internal format of the render buffer. */
        GLenum internalFormat;
    };

    /** Describes a frame buffer. */
    struct FrameBufferDescriptor
    {
        /** Holds descriptions for all textures used. */
        std::vector<TextureDescriptor> texDesc;
        /** Holds descriptions for all render buffers used. */
        std::vector<RenderBufferDescriptor> rbDesc;
    };

    /**
     * @brief  Represents frame buffers.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.17
     */
    class FrameBuffer
    {
    public:
        FrameBuffer();
        FrameBuffer(unsigned int fbWidth, unsigned int fbHeight, const FrameBufferDescriptor& desc);
        FrameBuffer(const FrameBuffer& orig);
        FrameBuffer(FrameBuffer&& orig);
        FrameBuffer& operator=(const FrameBuffer& orig);
        FrameBuffer& operator=(FrameBuffer&& orig);
        ~FrameBuffer();

        void UseAsRenderTarget();
        void Resize(unsigned int fbWidth, unsigned int fbHeight);

        unsigned int GetWidth() const { return width; };
        unsigned int GetHeight() const { return height; };

    private:
        void Destroy();

        unsigned int findAttachment(GLenum internalFormat, unsigned int& colorAtt, std::vector<GLenum> &drawBuffers);

        /** holds the frame buffers OpenGL name. */
        GLuint fbo;
        /** Holds whether this represents the back buffer or not. */
        bool isBackbuffer;
        /** holds the frame buffer type. */
        FrameBufferDescriptor desc;
        /** holds the frame buffers textures to render to. */
        std::vector<std::unique_ptr<GLTexture>> textures;
        /** holds the frame buffers render buffers to render to. */
        std::vector<GLuint> renderBuffers;
        /** holds the frame buffers width. */
        unsigned int width;
        /** holds the frame buffers height. */
        unsigned int height;
    };
}
#endif /* FRAMEBUFFER_H */
