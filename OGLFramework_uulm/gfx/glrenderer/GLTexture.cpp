/**
 * @file   GLTexture.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.08
 *
 * @brief  Contains the implementation of GLTexture.
 */

#include "GLTexture.h"
#include <FreeImage.h>

#undef min
#undef max

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
        depth(arraySize),
        hasMipMaps(false)
    {
        OGL_CALL(glGenTextures, 1, &id.textureId);
        OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, id.textureId);
        OGL_CALL(glTexStorage3D, GL_TEXTURE_2D_ARRAY, 1, descriptor.internalFormat, width, height, depth);
        OGL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, 0);
        InitSampling();
    }

    GLTexture::GLTexture(unsigned int size, const TextureDescriptor& desc) :
        id{ 0, GL_TEXTURE_1D },
        descriptor(desc),
        width(size),
        height(1),
        depth(1),
        hasMipMaps(false)
    {
        OGL_CALL(glGenTextures, 1, &id.textureId);
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexStorage1D, id.textureType, 1, descriptor.internalFormat, width);
        OGL_CALL(glBindTexture, id.textureType, 0);
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
        depth(1),
        hasMipMaps(false)
    {
        OGL_CALL(glGenTextures, 1, &id.textureId);
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexStorage2D, id.textureType, 1, descriptor.internalFormat, width, height);
        if (data) {
            OGL_CALL(glTexSubImage2D, id.textureType, 0, 0, 0, width, height, descriptor.format,
                descriptor.type, data);
        }
        OGL_CALL(glBindTexture, id.textureType, 0);
        InitSampling();
    }

    /**
    * Constructor.
    * Creates a 3d texture.
    * @param w the textures width
    * @param h the textures height
    * @param d the textures depth
    * @param desc the textures format
    * @param data the textures data
    */
    GLTexture::GLTexture(unsigned int w, unsigned int h, unsigned int d, const TextureDescriptor& desc, const void* data) :
        id{ 0, GL_TEXTURE_3D },
        descriptor(desc),
        width(w),
        height(h),
        depth(d),
        hasMipMaps(false)
    {
        OGL_CALL(glGenTextures, 1, &id.textureId);
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexStorage3D, id.textureType, 1, descriptor.internalFormat, width, height, depth);
        if (data) {
            OGL_CALL(glTexSubImage3D, id.textureType, 0, 0, 0, 0, width, height, depth,
                descriptor.format, descriptor.type, data);
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
        depth(0),
        hasMipMaps(false)
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        GLint qResult;
        OGL_CALL(glGetTexLevelParameteriv, id.textureType, 0, GL_TEXTURE_WIDTH, &qResult);
        width = static_cast<unsigned int>(qResult);
        OGL_CALL(glGetTexLevelParameteriv, id.textureType, 0, GL_TEXTURE_HEIGHT, &qResult);
        height = static_cast<unsigned int>(qResult);
        OGL_CALL(glGetTexLevelParameteriv, id.textureType, 0, GL_TEXTURE_DEPTH, &qResult);
        depth = static_cast<unsigned int>(qResult);
        InitSampling();
    }

    /** Destructor. */
    GLTexture::~GLTexture()
    {
        if (id.textureId != 0) {
            OGL_CALL(glBindTexture, id.textureType, 0);
            OGL_CALL(glDeleteTextures, 1, &id.textureId);
            id.textureId = 0;
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
     *  Activate the texture as an image.
     *  @param imageUnitIndex the index of the image unit to activate the texture for
     *  @param mipLevel the MipMap level to bind
     *  @param accessType the access type needed
     */
    void GLTexture::ActivateImage(GLuint imageUnitIndex, GLint mipLevel, GLenum accessType) const
    {
        OGL_CALL(glBindImageTexture, imageUnitIndex, id.textureId, mipLevel, GL_TRUE, 0, accessType, descriptor.internalFormat);
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
        switch (id.textureType)
        {
        case GL_TEXTURE_1D:
            OGL_CALL(glTexSubImage1D, id.textureType, 0, 0, width, descriptor.format, descriptor.type, data);
            break;
        case GL_TEXTURE_2D:
            OGL_CALL(glTexSubImage2D, id.textureType, 0, 0, 0, width, height, descriptor.format, descriptor.type, data);
            break;
        case GL_TEXTURE_3D:
            OGL_CALL(glTexSubImage3D, id.textureType, 0, 0, 0, 0, width, height, depth, descriptor.format, descriptor.type, data);
            break;
        default:
            throw std::runtime_error("Texture format not supported for upload.");
        }        
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /**
     *  Downloads the textures data to a vector.
     *  @param data the vector to contain the data.
     */
    void GLTexture::DownloadData(std::vector<uint8_t>& data)
    {
        data.resize(width * height * depth * descriptor.bytesPP);
        assert(data.size() != 0);

        // TODO: create external PBOs for real asynchronous up-/download [8/19/2015 Sebastian Maisch]
        GLuint pbo;
        OGL_CALL(glGenBuffers, 1, &pbo);
        OGL_CALL(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo);
        OGL_CALL(glBufferData, GL_PIXEL_PACK_BUFFER, data.size(), nullptr, GL_STREAM_READ);

        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glGetTexImage, id.textureType, 0, descriptor.format, descriptor.type, 0);

        OGL_CALL(glMemoryBarrier, GL_ALL_BARRIER_BITS);
        void* gpuMem = OGL_CALL(glMapBuffer, GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
        if (gpuMem) {
            memcpy(data.data(), gpuMem, data.size());
            OGL_CALL(glUnmapBuffer, GL_PIXEL_PACK_BUFFER);
        }

        OGL_CALL(glBindTexture, id.textureType, 0);
        OGL_CALL(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);
        OGL_CALL(glDeleteBuffers, 1, &pbo);
    }

    /**
     *  Uploads data to the texture from a vector.
     *  @param data the vector that contains the data.
     */
    void GLTexture::UploadData(std::vector<uint8_t>& data)
    {
        assert(data.size() != 0);

        // TODO: create external PBOs for real asynchronus up-/download [8/19/2015 Sebastian Maisch]
        GLuint pbo;
        OGL_CALL(glGenBuffers, 1, &pbo);
        OGL_CALL(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, pbo);
        OGL_CALL(glBufferData, GL_PIXEL_UNPACK_BUFFER, data.size(), nullptr, GL_STREAM_DRAW);

        void* gpuMem = OGL_CALL(glMapBuffer, GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        if (gpuMem) {
            memcpy(gpuMem, data.data(), data.size());
            OGL_CALL(glUnmapBuffer, GL_PIXEL_UNPACK_BUFFER);
        }

        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        if (id.textureType == GL_TEXTURE_3D || id.textureType == GL_TEXTURE_2D_ARRAY) {
            OGL_CALL(glTexSubImage3D, id.textureType, 0, 0, 0, 0, width, height, depth, descriptor.format, descriptor.type, 0);
        } else if (id.textureType == GL_TEXTURE_2D || id.textureType == GL_TEXTURE_1D_ARRAY) {
            OGL_CALL(glTexSubImage2D, id.textureType, 0, 0, 0, width, height, descriptor.format, descriptor.type, 0);
        } else {
            OGL_CALL(glTexSubImage1D, id.textureType, 0, 0, width, descriptor.format, descriptor.type, 0);
        }

        OGL_CALL(glBindTexture, id.textureType, 0);
        OGL_CALL(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, 0);
        OGL_CALL(glDeleteBuffers, 1, &pbo);
    }

    /**
     *  Generates MipMaps for the texture.
     */
    void GLTexture::GenerateMipMaps()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glGenerateMipmap, id.textureType);
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /**
     *  Generates min/max maps for the texture (only 4 components allowed, r: avg, g: min, b: max).
     *  @param minMaxProg the program for generating the Min/Max MipMaps
     *  @param uniformNames the names of the uniforms of the images used in the program
     */
    void GLTexture::GenerateMinMaxMaps(GPUProgram* minMaxProg, const std::vector<BindingLocation>& uniformNames)
    {
        assert(descriptor.format == GL_RGBA || descriptor.format == GL_RGBA_INTEGER);

        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glGenerateMipmap, id.textureType);

        unsigned int max_res = glm::max(width, glm::max(height, depth));
        glm::ivec3 firstMipRes = glm::ivec3(glm::ceil(glm::vec3(width, height, depth) * 0.5f));
        glm::ivec3 numGroups = glm::ivec3(glm::ceil(glm::vec3(firstMipRes) / 8.0f));
        unsigned int levels = glm::max(1, (int)glm::log2((float)max_res) + 1);

        minMaxProg->UseProgram();
        minMaxProg->SetUniform(uniformNames[0], 0);
        minMaxProg->SetUniform(uniformNames[1], 1);
        for (unsigned int i = 1; i < static_cast<unsigned int>(levels - 1); ++i) {
            numGroups = glm::ivec3(glm::ceil(glm::vec3(numGroups) * 0.5f));
            ActivateImage(0, i - 1, GL_READ_ONLY);
            ActivateImage(1, i, GL_WRITE_ONLY);
            OGL_CALL(glDispatchCompute, numGroups.x, numGroups.y, numGroups.z);
            OGL_CALL(glMemoryBarrier, GL_ALL_BARRIER_BITS);
            OGL_SCALL(glFinish);
        }
        hasMipMaps = true;
        InitSampling();
    }

    /**
     *  Sets the sampler parameters for mirroring.
     */
    void GLTexture::SampleWrapMirror()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        if (id.textureType == GL_TEXTURE_2D || id.textureType == GL_TEXTURE_3D) {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        }
        if (id.textureType == GL_TEXTURE_3D) {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
        }
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /**
     *  Sets the sampler parameters for clamping.
     */
    void GLTexture::SampleWrapClamp()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        if (id.textureType == GL_TEXTURE_2D || id.textureType == GL_TEXTURE_3D) {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        if (id.textureType == GL_TEXTURE_3D) {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        OGL_CALL(glBindTexture, id.textureType, 0);
    }

    /**
     *  Sets the sampler parameters for linear filtering.
     */
    void GLTexture::SampleLinear()
    {
        OGL_CALL(glBindTexture, id.textureType, id.textureId);
        if (hasMipMaps) {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_BASE_LEVEL, 0);
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
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_BASE_LEVEL, 0);
        } else {
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            OGL_CALL(glTexParameteri, id.textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        OGL_CALL(glBindTexture, id.textureType, 0);
    }
}