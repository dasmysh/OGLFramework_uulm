/**
 * @file   FrameBuffer.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Januar 2014
 *
 * @brief  Contains the definition of FrameBuffer.
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
#include <GL/glew.h>

/** The currently initialized frame buffer type. */
enum class FrameBufferType
{
    BACKBUFFER, /**< The rendertarget is the backbuffer. */
    BACKBUFFER_CLONE, /**< The rendertarget is a buffer for general geometry rendering (with depth buffer, color texture, ...). */
    SHADOWMAP, /**< The rendertarget is used as a shadow map. */
    POSTPROCESSING /**< The rendertarget is used for post processing steps. */
};

/**
 * @brief  Represents frame buffers.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Januar 2014
 */
class FrameBuffer
{
private:
    /** Deleted copy constructor. */
    FrameBuffer(const FrameBuffer&) {};
    /** Deleted copy assignment operator. */
    FrameBuffer& operator=(const FrameBuffer&) { return *this; };

public:
    FrameBuffer();
    FrameBuffer(unsigned int fbWidth, unsigned int fbHeight);
    virtual ~FrameBuffer();

    void UseAsRenderTarget();
    void Resize(unsigned int fbWidth, unsigned int fbHeight);
    void InitBackBufferClone();
    void InitShadowMap();
    void InitPostProcessing();

private:
    /** Frees all used textures. */
    void FreeTextures();

    /** holds the frame buffers OpenGL name. */
    GLuint fbo;
    /** holds the frame buffer type. */
    FrameBufferType type;
    /** holds the frame buffers textures to render to. */
    std::vector<GLuint> textures;
    /** holds the frame buffers render buffers to render to. */
    std::vector<GLuint> renderBuffers;

public:/** holds the frame buffers width. */
    unsigned int width;
    /** holds the frame buffers height. */
    unsigned int height;
};

#endif /* FRAMEBUFFER_H */
