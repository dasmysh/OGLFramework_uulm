/**
 * @file   GLTexture.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   8. Februar 2014
 *
 * @brief  Contains the definition of GLTexture.
 */

#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "main.h"

/**
 * @brief  General representation of an OpenGL texture.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   8. Februar 2014
 */
class GLTexture
{
private:
    /** Deleted copy constructor. */
    GLTexture(const GLTexture&) {};
    /** Deleted copy assignment operator. */
    GLTexture& operator=(const GLTexture&) { return *this; };

public:
    GLTexture(GLuint texID, GLenum texType);
    GLTexture(unsigned int width, unsigned int height, unsigned int bpp, unsigned int arraySize);
    GLTexture(unsigned int width, unsigned int height, unsigned int bpp, const void* data);
    virtual ~GLTexture();

    void ActivateTexture(GLenum textureUnit) const;
    void AddTextureToArray(const std::string& file, unsigned int slice);
    void SetData(const void* data);

    // TODO: sampler parameters?
    void SamplerWrapMirror();
    void SamplerWrapClamp();

    GLuint GetTextureName();
    GLenum GetTextureType();

private:
    /** Holds the OpenGL texture id. */
    GLuint textureId;
    /** Holds the type of the open gl texture. */
    GLenum textureType;
    /** Holds the pixel format of the texture. */
    GLenum pixelFormat;
    /** Holds the unsized pixel format of the texture. */
    GLenum pixelFormatUnsized;
    /** Holds the element type of the pixel format. */
    GLenum elementType;
    /** Holds the width. */
    unsigned int width;
    /** Holds the height. */
    unsigned int height;
    /** Holds the sampler for the texture. */
    GLuint sampler;

    void InitSampler();
    void CalcPixelFormat(unsigned int bpp);
};

#endif /* GLTEXTURE_H */
