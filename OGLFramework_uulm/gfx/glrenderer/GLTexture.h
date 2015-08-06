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

namespace cgu {
    class GLTexture;
    namespace gpgpu {
        class CUDAImage;
    }

    /** Describes the format of a texture. */
    struct TextureDescriptor
    {
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
    private:
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
    private:
        /** Deleted copy constructor. */
        GLTexture(const GLTexture&) = delete;
        /** Deleted copy assignment operator. */
        GLTexture& operator=(const GLTexture&)  = delete;

    public:
        GLTexture(GLuint texID, GLenum texType, const TextureDescriptor& desc);
        GLTexture(unsigned int width, unsigned int height, unsigned int arraySize, const TextureDescriptor& desc);
        GLTexture(unsigned int width, unsigned int height, const TextureDescriptor& desc, const void* data);
        virtual ~GLTexture();

        void ActivateTexture(GLenum textureUnit) const;
        void AddTextureToArray(const std::string& file, unsigned int slice);
        void SetData(const void* data);
        void GenerateMipMaps();

        void SampleWrapMirror();
        void SampleWrapClamp();
        void SampleLinear();
        void SampleNearest();

        const TextureGLIdentifierAccessor& GetGLIdentifier() const { return id; };

    private:
        /** Holds the OpenGL texture id. */
        TextureGLIdentifierAccessor id;
        /** Holds the texture descriptor. */
        TextureDescriptor descriptor;

        //GLuint textureId;
        /** Holds the type of the open gl texture. */
        //GLenum textureType;
        /** Holds the pixel format of the texture. */
        //GLenum pixelFormat;
        /** Holds the unsized pixel format of the texture. */
        //GLenum pixelFormatUnsized;
        /** Holds the element type of the pixel format. */
        //GLenum elementType;
        /** Holds the width. */
        unsigned int width;
        /** Holds the height. */
        unsigned int height;
        /** Holds whether the texture has mip maps. */
        bool hasMipMaps;

        void InitSampling();
    };
}

#endif /* GLTEXTURE_H */
