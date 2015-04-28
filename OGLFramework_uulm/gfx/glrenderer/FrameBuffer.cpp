/**
 * @file   FrameBuffer.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Januar 2014
 *
 * @brief  Contains the implementation of FrameBuffer.
 */

#include "FrameBuffer.h"
#include <exception>
#include <stdexcept>
#include "../../main.h"

/**
 * Constructor.
 * Creates a FrameBuffer representing the backbuffer.
 */
FrameBuffer::FrameBuffer() :
fbo(0),
type(FrameBufferType::BACKBUFFER),
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
 */
FrameBuffer::FrameBuffer(unsigned int fbWidth, unsigned int fbHeight) :
fbo(0),
type(FrameBufferType::BACKBUFFER),
width(fbWidth),
height(fbHeight)
{
}

FrameBuffer::~FrameBuffer()
{
    FreeTextures();
    if (fbo != 0) {
        OGL_CALL(glDeleteFramebuffers, 1, &fbo);
        fbo = 0;
    }
}

/**
 * Releases all textures used in the current frame buffer object.
 */
void FrameBuffer::FreeTextures()
{
    for (GLuint tex : textures) {
        if (tex != 0) {
            OGL_CALL(glDeleteTextures, 1, &tex);
        }
    }
    for (GLuint rb : renderBuffers) {
        if (rb != 0) {
            OGL_CALL(glDeleteRenderbuffers, 1, &rb);
        }
    }
    textures.clear();
    renderBuffers.clear();
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

    switch (type)
    {
        case FrameBufferType::BACKBUFFER:
            break;
        case FrameBufferType::BACKBUFFER_CLONE:
            InitBackBufferClone();
            break;
        case FrameBufferType::SHADOWMAP:
            InitShadowMap();
            break;
        case FrameBufferType::POSTPROCESSING:
            InitPostProcessing();
            break;
        default:
            throw std::runtime_error("FrameBufferType not implemented!");
            break;
    }
}

/**
 * Use this frame buffer object as target for rendering.
 */
void FrameBuffer::UseAsRenderTarget()
{
    OGL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo);
    OGL_CALL(glViewport, 0, 0, width, height);
}

/**
 * Initialize this frame buffer as a backbuffer clone (color texture + depth buffer).
 */
void FrameBuffer::InitBackBufferClone()
{
    this->FreeTextures();

    OGL_CALL(glGenFramebuffers, 1, &fbo);
    type = FrameBufferType::BACKBUFFER_CLONE;
    OGL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo);
    GLuint colorTex;
    OGL_CALL(glGenTextures, 1, &colorTex);
    OGL_CALL(glBindTexture, GL_TEXTURE_2D, colorTex);
    OGL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA32UI, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLuint depthBuffer;
    OGL_CALL(glGenRenderbuffers, 1, &depthBuffer);
    OGL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, depthBuffer);
    OGL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    OGL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    OGL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTex, 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    OGL_CALL(glDrawBuffers, 1, DrawBuffers);

    GLenum fboStatus = OGL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Could not create Framebuffer.");

    textures.push_back(colorTex);
    renderBuffers.push_back(depthBuffer);
}

/**
 * Initialize this frame buffer object as a shadow map.
 */
void FrameBuffer::InitShadowMap()
{
    OGL_CALL(glGenFramebuffers, 1, &fbo);
    type = FrameBufferType::SHADOWMAP;
    OGL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo);
    GLuint depthTex;
    OGL_CALL(glGenTextures, 1, &depthTex);
    OGL_CALL(glBindTexture, GL_TEXTURE_2D, depthTex);
    OGL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    OGL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    OGL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0);
    OGL_CALL(glDrawBuffer, GL_NONE);

    GLenum fboStatus = OGL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Could not create Framebuffer.");

    textures.push_back(depthTex);
}

/**
 * Initialize this frame buffer object as target for post processing steps.
 */
void FrameBuffer::InitPostProcessing()
{
    throw std::runtime_error("FrameBufferType not implemented!");
}

