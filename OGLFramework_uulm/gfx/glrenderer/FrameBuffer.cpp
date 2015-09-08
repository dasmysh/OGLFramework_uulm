/**
 * @file   FrameBuffer.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.17
 *
 * @brief  Contains the implementation of FrameBuffer.
 */

#include "FrameBuffer.h"
#include <exception>
#include <stdexcept>
#include "../../main.h"

namespace cgu {
    /**
     * Constructor.
     * Creates a FrameBuffer representing the backbuffer.
     */
    FrameBuffer::FrameBuffer() :
        fbo(0),
        isBackbuffer(true),
        desc(),
        textures(),
        renderBuffers(),
        width(0),
        height(0)
    {
    }

    /**
     * Constructor.
     * Creates a new FrameBuffer with given width and height. It is initialized as backbuffer as default.
     * @param fbWidth the frame buffers width
     * @param fbHeight the frame buffers height.
     * @param d the frame buffers description.
     */
    FrameBuffer::FrameBuffer(unsigned int fbWidth, unsigned int fbHeight, const FrameBufferDescriptor& d) :
        fbo(0),
        isBackbuffer(false),
        desc(d)
    {
        Resize(fbWidth, fbHeight);
    }

    /**
     *  Copy constructor for a frame buffer.
     *  @param orig the original frame buffer.
     */
    FrameBuffer::FrameBuffer(const FrameBuffer& orig) :
        fbo(0),
        isBackbuffer(false),
        desc(orig.desc)
    {
        Resize(orig.width, orig.height);
    }

    /**
     *  Move-Constructs a frame buffer object.
     *  @param orig the original frame buffer.
     */
    FrameBuffer::FrameBuffer(FrameBuffer&& orig) :
        fbo(orig.fbo),
        isBackbuffer(orig.isBackbuffer),
        desc(std::move(orig.desc)),
        textures(std::move(orig.textures)),
        renderBuffers(std::move(orig.renderBuffers)),
        width(orig.width),
        height(orig.height)
    {
        orig.fbo = 0;
        orig.isBackbuffer = false;
        orig.desc = FrameBufferDescriptor();
        orig.textures.clear();
        orig.renderBuffers.clear();
        orig.width = 0;
        orig.height = 0;
    }

    /**
     *  Assigns a copy of another frame buffer.
     *  @param orig the original frame buffer.
     */
    FrameBuffer& FrameBuffer::operator=(const FrameBuffer& orig)
    {
        Destroy();
        fbo = 0;
        isBackbuffer = orig.isBackbuffer;
        desc = orig.desc;
        Resize(orig.width, orig.height);
        return *this;
    }

    /**
     *  Assigns another frame buffer by moving its contents.
     *  @param orig the original frame buffer.
     */
    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& orig)
    {
        Destroy();
        fbo = orig.fbo;
        orig.fbo = 0;
        isBackbuffer = orig.isBackbuffer;
        orig.isBackbuffer = false;
        desc = orig.desc;
        orig.desc = FrameBufferDescriptor();
        textures = std::move(orig.textures);
        orig.textures.clear();
        renderBuffers = std::move(orig.renderBuffers);
        orig.renderBuffers.clear();
        width = orig.width;
        orig.width = 0;
        height = orig.height;
        orig.height = 0;
        return *this;
    }

    /**
     *  Destructor.
     */
    FrameBuffer::~FrameBuffer()
    {
        Destroy();
    }

    /**
     *  Destroys the frame buffer object.
     */
    void FrameBuffer::Destroy()
    {
        for (auto& tex : textures) {
            tex.release();
        }
        for (GLuint rb : renderBuffers) {
            if (rb != 0) {
                OGL_CALL(glDeleteRenderbuffers, 1, &rb);
            }
        }
        textures.clear();
        renderBuffers.clear();

        if (fbo != 0) {
            OGL_CALL(glDeleteFramebuffers, 1, &fbo);
            fbo = 0;
        }
    }

    /**
     * Resizes the frame buffer and re-initializes it if needed.
     * @param fbWidth the new width
     * @param fbHeight the new height
     */
    void FrameBuffer::Resize(unsigned int fbWidth, unsigned int fbHeight)
    {
        width = fbWidth;
        height = fbHeight;

        this->Destroy();

        if (isBackbuffer) return;

        OGL_CALL(glGenFramebuffers, 1, &fbo);
        OGL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo);
        unsigned int colorAtt = 0;
        drawBuffers.clear();
        for (const auto& texDesc : desc.texDesc) {
            GLuint tex;
            OGL_CALL(glGenTextures, 1, &tex);
            OGL_CALL(glBindTexture, GL_TEXTURE_2D, tex);
            OGL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, texDesc.internalFormat, width, height, 0, texDesc.format, texDesc.type, nullptr);
            std::unique_ptr<GLTexture> texture{ new GLTexture{ tex, GL_TEXTURE_2D, texDesc } };

            GLenum attachment = findAttachment(texDesc.internalFormat, colorAtt, drawBuffers);
            OGL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, attachment, tex, 0);
            textures.push_back(std::move(texture));
        }


        for (const auto& rbDesc : desc.rbDesc) {
            GLuint rb;
            OGL_CALL(glGenRenderbuffers, 1, &rb);
            OGL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rb);
            OGL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, rbDesc.internalFormat, width, height);
            GLenum attachment = findAttachment(rbDesc.internalFormat, colorAtt, drawBuffers);
            OGL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rb);
            renderBuffers.push_back(rb);
        }

        OGL_CALL(glDrawBuffers, static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

        GLenum fboStatus = OGL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Could not create frame buffer.");
    }

    /**
     * Use this frame buffer object as target for rendering.
     */
    void FrameBuffer::UseAsRenderTarget()
    {
        OGL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo);
        if (!isBackbuffer) OGL_CALL(glDrawBuffers, static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
        OGL_CALL(glViewport, 0, 0, width, height);
    }

    /**
    * Use this frame buffer object as target for rendering and select the draw buffers used.
    * @param drawBufferIndices the indices in the draw buffer to be used.
    */
    void FrameBuffer::UseAsRenderTarget(const std::vector<unsigned int> drawBufferIndices)
    {
        assert(!isBackbuffer);
        std::vector<GLenum> drawBuffersReduced(drawBuffers.size());
        for (unsigned int i = 0; i < drawBufferIndices.size(); ++i) drawBuffersReduced[i] = drawBuffers[drawBufferIndices[i]];

        OGL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo);
        OGL_CALL(glDrawBuffers, static_cast<GLsizei>(drawBuffersReduced.size()), drawBuffersReduced.data());        
        OGL_CALL(glViewport, 0, 0, width, height);
    }

    unsigned int FrameBuffer::findAttachment(GLenum internalFormat, unsigned int& colorAtt, std::vector<GLenum> &drawBuffers)
    {
        GLenum attachment;
        switch (internalFormat)
        {
        case GL_DEPTH_STENCIL:
            attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT32:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            attachment = GL_DEPTH_ATTACHMENT;
            break;
        case GL_STENCIL_INDEX:
        case GL_STENCIL_INDEX1:
        case GL_STENCIL_INDEX4:
        case GL_STENCIL_INDEX8:
        case GL_STENCIL_INDEX16:
            attachment = GL_STENCIL_ATTACHMENT;
            break;
        default:
            attachment = GL_COLOR_ATTACHMENT0 + colorAtt++;
            drawBuffers.push_back(attachment);
            break;
        }
        return attachment;
    }

}
