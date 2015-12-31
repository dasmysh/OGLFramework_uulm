/**
 * @file   GLTexture.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.08
 *
 * @brief  Contains the definition of GLTexture.
 */

#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "main.h"
#include "GPUProgram.h"

namespace cgu {
    class GLTexture;
    class FrameBuffer;

    namespace gpgpu {
        class CUDAImage;
    }

    /** Describes the format of a texture. */
    struct TextureDescriptor
    {
        TextureDescriptor(unsigned int btsPP, GLint intFmt, GLenum fmt, GLenum tp) : bytesPP(btsPP), internalFormat(intFmt), format(fmt), type(tp) {};

        /** Holds the bytes per pixel of the format. */
        unsigned int bytesPP;
        /** Holds the internal format. */
        GLint internalFormat;
        /** Holds the format. */
        GLenum format;
        /** Holds the type. */
        GLenum type;
    };

    class TextureGLIdentifierAccessor
    {
        friend class cgu::gpgpu::CUDAImage;
        friend class cgu::GLTexture;
        friend class cgu::FrameBuffer;

        TextureGLIdentifierAccessor(GLuint id, GLenum type) : textureId(id), textureType(type) {};
        GLuint textureId;
        GLenum textureType;
    };

    /**
    * @brief  General representation of an OpenGL texture.
    *
    * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
    * @date   2014.02.08
    */
    class GLTexture
    {
        /** Deleted copy constructor. */
        GLTexture(const GLTexture&) = delete;
        /** Deleted copy assignment operator. */
        GLTexture& operator=(const GLTexture&)  = delete;

    public:
        GLTexture(GLuint texID, GLenum texType, const TextureDescriptor& desc);
        GLTexture(unsigned int size, const TextureDescriptor& desc);
        GLTexture(unsigned int width, unsigned int height, unsigned int arraySize, const TextureDescriptor& desc);
        GLTexture(unsigned int width, unsigned int height, const TextureDescriptor& desc, const void* data);
        GLTexture(unsigned int width, unsigned int height, unsigned int depth, const TextureDescriptor& desc, const void* data);
        virtual ~GLTexture();

        void ActivateTexture(GLenum textureUnit) const;
        void ActivateImage(GLuint imageUnitIndex, GLint mipLevel, GLenum accessType) const;
        void AddTextureToArray(const std::string& file, unsigned int slice) const;
        void SetData(const void* data) const;
        void DownloadData(std::vector<uint8_t>& data) const;
        void UploadData(std::vector<uint8_t>& data) const;
        void GenerateMipMaps() const;
        void GenerateMinMaxMaps(GPUProgram* minMaxProgram, const std::vector<BindingLocation>& uniformNames);

        void SampleWrapMirror() const;
        void SampleWrapClamp() const;
        void SampleLinear() const;
        void SampleNearest() const;

        const TextureGLIdentifierAccessor& GetGLIdentifier() const { return id; };

    private:
        /** Holds the OpenGL texture id. */
        TextureGLIdentifierAccessor id;
        /** Holds the texture descriptor. */
        TextureDescriptor descriptor;

        /** Holds the width. */
        unsigned int width;
        /** Holds the height. */
        unsigned int height;
        /** Holds the depth or number of array slices. */
        unsigned int depth;
        /** Holds whether the texture has mip maps. */
        bool hasMipMaps;

        void InitSampling() const;
    };
}

#endif /* GLTEXTURE_H */
