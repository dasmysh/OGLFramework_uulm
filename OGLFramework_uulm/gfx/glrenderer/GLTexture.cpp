/**
 * @file   GLTexture.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   8. Februar 2014
 *
 * @brief  Contains the implementation of GLTexture.
 */

#include "GLTexture.h"
#include <FreeImage.h>

/**
 * Constructor.
 * Creates a 2d array texture.
 * @param w the textures width
 * @param h the textures height
 * @param bpp the textures bits per pixel
 * @param arraySize the array size
 */
GLTexture::GLTexture(unsigned int w, unsigned int h, unsigned int bpp,
        unsigned int arraySize) :
textureId(0),
textureType(GL_TEXTURE_2D_ARRAY),
pixelFormat(GL_RGBA8),
pixelFormatUnsized(GL_RGBA),
width(w),
height(h),
sampler(0)
{
    CalcPixelFormat(bpp);
    OGL_CALL(glGenTextures, 1, &textureId);
    OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, textureId);
    OGL_CALL(glTexStorage3D, GL_TEXTURE_2D_ARRAY, 1, pixelFormat, width, height, arraySize);
    OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, 0);
    InitSampler();
}

/**
 * Constructor.
 * Creates a 2d texture.
 * @param w the textures width
 * @param h the textures height
 * @param bpp the textures bits per pixel
 * @param data the textures data
 */
GLTexture::GLTexture(unsigned int w, unsigned int h, unsigned int bpp, const void* data) :
textureId(0),
textureType(GL_TEXTURE_2D),
pixelFormat(GL_RGBA8),
pixelFormatUnsized(GL_RGBA),
elementType(GL_UNSIGNED_BYTE),
width(w),
height(h),
sampler(0)
{
    CalcPixelFormat(bpp);
    OGL_CALL(glGenTextures, 1, &textureId);
    OGL_CALL(glBindTexture, GL_TEXTURE_2D, textureId);
    OGL_CALL(glTexStorage2D, GL_TEXTURE_2D, 1, pixelFormat, width, height);
    if (data) {
        OGL_CALL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, width, height, pixelFormatUnsized,
            elementType, data);
    }
    OGL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    InitSampler();
}

/**
 * Manages a pre-created texture.
 * @param texID the texture id
 * @param texType the textures type
 */
GLTexture::GLTexture(GLuint texID, GLenum texType) :
textureId(texID),
textureType(texType),
pixelFormat(GL_RGBA8),
pixelFormatUnsized(GL_RGBA),
width(0),
height(0),
sampler(0)
{
    InitSampler();
}

/** Destructor. */
GLTexture::~GLTexture()
{
    if (sampler != 0) {
        OGL_CALL(glDeleteSamplers, 1, &sampler);
        sampler = 0;
    }

    if (textureId != 0) {
        OGL_CALL(glDeleteTextures, 1, &textureId);
        textureId = 0;
    }
}

/** Initializes the sampler. */
void GLTexture::InitSampler()
{
    OGL_CALL(glGenSamplers, 1, &sampler);
    OGL_CALL(glSamplerParameteri, sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    OGL_CALL(glSamplerParameteri, sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    OGL_CALL(glSamplerParameterf, sampler, GL_TEXTURE_MIN_LOD, 0.0f);
    OGL_CALL(glSamplerParameterf, sampler, GL_TEXTURE_MAX_LOD, 0.0f);
}

/**
 * Calculates the pixel formats.
 * @param bpp the bits per pixel to calc the format for
 * @todo more formats in the future? would propably need one more parameter
 */
void GLTexture::CalcPixelFormat(unsigned int bpp)
{
    if (bpp == 128) {
        pixelFormat = GL_RGBA32F;
        pixelFormatUnsized = GL_RGBA;
        elementType = GL_FLOAT;
    } else if (bpp == 32) {
        pixelFormat = GL_RGBA8;
        pixelFormatUnsized = GL_RGBA;
    } else if (bpp == 24) {
        pixelFormat = GL_RGB8;
        pixelFormatUnsized = GL_RGB;
    } else if (bpp == 8) {
        pixelFormat = GL_R8;
        pixelFormatUnsized = GL_RED;
    } else {
        pixelFormat = GL_RGB;
        pixelFormatUnsized = GL_RGB;
    }
}

/**
 * Activate the texture for rendering
 * @param textureUnit the texture unit to set the texture to
 */
void GLTexture::ActivateTexture(GLenum textureUnit) const
{
    OGL_CALL(glActiveTexture, textureUnit);
    OGL_CALL(glBindTexture, textureType, textureId);
    OGL_CALL(glBindSampler, textureUnit - GL_TEXTURE0, sampler);
}

/**
 * Add a 2d bitmap to a texture array.
 * @param file the file to add
 * @param slice the array slice to add the files content to
 */
void GLTexture::AddTextureToArray(const std::string& file, unsigned int slice)
{
    FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, file.c_str());
    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    unsigned int texWidth = FreeImage_GetWidth(bitmap32);
    unsigned int texHeight = FreeImage_GetHeight(bitmap32);
    if (width != texWidth || height != texHeight) {
        LOG(ERROR) << L"Texture \"" << file.c_str() << L"\" has the wrong format!";
        FreeImage_Unload(bitmap32);
        FreeImage_Unload(bitmap);
        throw std::runtime_error("Texture \"" + file + "\" has the wrong format.");
    }
    void* data = FreeImage_GetBits(bitmap32);

    OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, textureId);
    OGL_CALL(glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, 0, 0, slice, width, height, 1,
            pixelFormatUnsized, GL_UNSIGNED_BYTE, data);
    OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, 0);

    FreeImage_Unload(bitmap32);
    FreeImage_Unload(bitmap);
}

void GLTexture::SetData(const void* data)
{
    OGL_CALL(glBindTexture, GL_TEXTURE_2D, textureId);
    OGL_CALL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, width, height, pixelFormatUnsized,
            elementType, data);
}

/// <summary>
/// Gets the name of the texture.
/// </summary>
/// <returns>The name of the texture.</returns>
GLuint GLTexture::GetTextureName()
{
    return textureId;
}

/// @brief  Gets the texture type.
///
/// @author Sebastian Maisch
/// @date   02.04.2014
///
/// @return The texture type.
GLenum GLTexture::GetTextureType()
{
    return textureType;
}

void GLTexture::SamplerWrapMirror()
{
    OGL_CALL(glSamplerParameteri, sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    OGL_CALL(glSamplerParameteri, sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}

void GLTexture::SamplerWrapClamp()
{
    OGL_CALL(glSamplerParameteri, sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    OGL_CALL(glSamplerParameteri, sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
