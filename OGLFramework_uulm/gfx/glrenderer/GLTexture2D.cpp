/**
 * @file   GLTexture2D.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 *
 * @brief  Contains the implementation of GLTexture2D.
 */

#include "GLTexture2D.h"
#include "GLTexture.h"
#include <GL/gl.h>
#include <FreeImage.h>
#include <boost/algorithm/string/predicate.hpp>
#include "app/ApplicationBase.h"
#include "app/Configuration.h"

namespace cgu {

    /**
     * Constructor.
     * @param texFilename the textures file name
     */
    GLTexture2D::GLTexture2D(const std::string& texFilename, ApplicationBase* app) :
        Resource{ texFilename, app },
        texture()
    {
    }

    GLTexture2D::~GLTexture2D()
    {
        if (IsLoaded()) UnloadLocal();
    }

    void GLTexture2D::Load()
    {
        std::string filename = application->GetConfig().resourceBase + "/" + id;
        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str());
        int flags = 0;

        if (format == FIF_JPEG) {
            flags = JPEG_ACCURATE;
        } else if (format == FIF_TARGA) {
            flags = TARGA_LOAD_RGB888;
        }

        FIBITMAP* bitmap = FreeImage_Load(format, filename.c_str(), flags);
        FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
        unsigned int width = FreeImage_GetWidth(bitmap32);
        unsigned int height = FreeImage_GetHeight(bitmap32);
        unsigned int redMask = FreeImage_GetRedMask(bitmap32);
        unsigned int greenMask = FreeImage_GetGreenMask(bitmap32);
        unsigned int blueMask = FreeImage_GetBlueMask(bitmap32);
        void* data = FreeImage_GetBits(bitmap32);
        GLenum fmt = GL_RGBA;
        if (redMask > greenMask && greenMask > blueMask) fmt = GL_BGRA;
        TextureDescriptor texDesc(4, GL_RGBA8, fmt, GL_UNSIGNED_BYTE);
        texture = std::unique_ptr<GLTexture>(new GLTexture(width, height, texDesc, data));
        FreeImage_Unload(bitmap32);
        FreeImage_Unload(bitmap);
        Resource::Load();
    }

    /**
     * Unloads the local resources.
     */
    void GLTexture2D::UnloadLocal()
    {
        texture.reset();
    }

    /** Returns the texture object. */
    GLTexture* GLTexture2D::GetTexture()
    {
        return texture.get();
    }

    /** Returns the texture object. */
    const GLTexture* GLTexture2D::GetTexture() const
    {
        return texture.get();
    }

    void GLTexture2D::Unload()
    {
        UnloadLocal();
        Resource::Unload();
    }
}
