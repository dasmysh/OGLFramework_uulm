/**
 * @file   GLTexture.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.08
 *
 * @brief  Contains the implementation of GLTexture.
 */

#include "GLTexture.h"
#include <FreeImage.h>

namespace cgu {

    /**
     * Constructor.
     * Creates a 2d array texture.
     * @param w the textures width
     * @param h the textures height
     * @param desc the textures format
     * @param arraySize the array size
     */
    GLTexture::GLTexture(unsigned int w, unsigned int h, unsigned int arraySize, const TextureDescriptor& desc) :
        id{ 0, GL_TEXTURE_2D_ARRAY },
        descriptor(desc),
        width(w),
        height(h),
        hasMipMaps(false)
    {
        OGL_CALL(glGenTextures, 1, &id.textureId);
        OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, id.textureId);
        OGL_CALL(glTexStorage3D, GL_TEXTURE_2D_ARRAY, 1, descriptor.internalFormat, width, height, arraySize);
        OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, 0);
        InitSampling();
    }

    /**
     * Constructor.
     * Creates a 2d texture.
     * @param w the textures width
     * @param h the textures height
     * @param desc the textures format
     * @param data the textures data
     */
    GLTexture::GLTexture(unsigned int w, unsigned int h, const TextureDescriptor& desc, const void* data) :
        id{ 0, GL_TEXTURE_2D },
        descriptor(desc),
        width(w),
        height(h),
        hasMipMaps(false)
    {
        OGL_CALL(glGenTextures, 1, &id.textureType);
        OGL_CALL(glBindTexture, id.textureType, id.textureType);
        OGL_CALL(glTexStorage2D, id.textureType, 1, descriptor.internalFormat, width, height);
        if (data) {
            OGL_CALL(glTexSubImage2D, id.textureType, 0, 0, 0, width, height, descriptor.format,
                descriptor.type, data);
        }
        OGL_CALL(glBindTexture, id.textureType, 0);
        InitSampling();
    }

    /**
     * Manages a pre-created texture.
     * @param texID the texture id
     * @param texType the textures type
     */
    GLTexture::GLTexture(GLuint texID, GLenum texType, const TextureDescriptor& desc) :
        id{ texID, texType },
        descriptor(desc),
        width(0),
        height(0),
        hasMipMaps(false)
    {
        InitSampling();
    }

    /** Destructor. */
    GLTexture::~GLTexture()
    {
        if (id.textureType != 0) {
            OGL_CALL(glDeleteTextures, 1, &id.textureType);
            id.textureType = 0;
        }
    }

    /** Initializes the sampler. */
    void GLTexture::InitSampling()
    {
        SampleLinear();
        SampleWrapClamp();
    }

    /**
     * Activate the texture for rendering
     * @param textureUnit the texture unit to set the texture to
     */
    void GLTexture::ActivateTexture(GLenum textureUnit) const
    {
        OGL_CALL(glActiveTexture, textureUnit);
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
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

        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexSubImage3D, id.textureType, 0, 0, 0, slice, width, height, 1,
            descriptor.format, descriptor.type, data);
        OGL_CALL(glBindTexture, id.textureType, 0);

        FreeImage_Unload(bitmap32);
        FreeImage_Unload(bitmap);
    }

    /**
     *  Sets the textures data.
     *  @param data the data to set.
     */
    void GLTexture::SetData(const void* data)
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexSubImage2D, id.textureType, 0, 0, 0, width, height, descriptor.format,
            descriptor.type, data);
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    void GLTexture::GenerateMipMaps()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glGenerateMipmap, id.textureType);
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /// <summary>
    /// Gets the name of the texture.
    /// </summary>
    /// <returns>The name of the texture.</returns>
    /*GLuint GLTexture::GetTextureName()
    {
        return textureId;
    }*/

    /// @brief  Gets the texture type.
    ///
    /// @author Sebastian Maisch
    /// @date   02.04.2014
    ///
    /// @return The texture type.
    /*GLenum GLTexture::GetTextureType()
    {
        return textureType;
    }*/

    /**
     *  Sets the sampler parameters for mirroring.
     */
    void GLTexture::SampleWrapMirror()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        if (id.textureType == GL_TEXTURE_2D || id.textureType == GL_TEXTURE_3D)
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        if (id.textureType == GL_TEXTURE_3D)
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /**
     *  Sets the sampler parameters for clamping.
     */
    void GLTexture::SampleWrapClamp()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        if (id.textureType == GL_TEXTURE_2D || id.textureType == GL_TEXTURE_3D)
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        if (id.textureType == GL_TEXTURE_3D)
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /**
     *  Sets the sampler parameters for linear filtering.
     */
    void GLTexture::SampleLinear()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        if (hasMipMaps) {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /**
     *  Sets the sampler parameters for box filtering.
     */
    void GLTexture::SampleNearest()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        if (hasMipMaps) {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        } else {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        OGL_CALL(glBindTexture, id.textureType, 0);
    }
}