/**
 * @file   GLTexture2D.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2013.12.31
 *
 * @brief  Contains the implementation of GLTexture2D.
 */

#include "GLTexture2D.h"
#include "GLTexture.h"
#include <GL/gl.h>
#include <FreeImage.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
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

    /** Copy constructor. */
    GLTexture2D::GLTexture2D(const GLTexture2D& rhs) : GLTexture2D(rhs.id, rhs.application)
    {
        if (rhs.IsLoaded()) GLTexture2D::Load();
    }

    /** Copy assignment operator. */
    GLTexture2D& GLTexture2D::operator=(const GLTexture2D& rhs)
    {
        auto tmp(rhs);
        std::swap(texture, tmp.texture);
        return *this;
    }

    /** Default move constructor. */
    GLTexture2D::GLTexture2D(GLTexture2D&& rhs) : Resource(std::move(rhs)), texture(std::move(rhs.texture)) {}

    /** Default move assignment operator. */
    GLTexture2D& GLTexture2D::operator=(GLTexture2D&& rhs)
    {
        Resource* tRes = this;
        *tRes = static_cast<Resource&&>(std::move(rhs));
        texture = std::move(rhs.texture);
        return *this;
    }

    /** Destructor. */
    GLTexture2D::~GLTexture2D()
    {
        if (IsLoaded()) UnloadLocal();
    }

    void GLTexture2D::Load()
    {
        auto fileOptions = GetParameters();
        auto filename = application->GetConfig().resourceBase + "/" + fileOptions[0];
        if (!boost::filesystem::exists(filename)) {
            LOG(ERROR) << "File \"" << filename.c_str() << L"\" cannot be opened.";
            throw resource_loading_error() << ::boost::errinfo_file_name(filename) << resid_info(id)
                << errdesc_info("Cannot open include file.");
        }

        auto format = FreeImage_GetFileType(filename.c_str());
        auto flags = 0;

        if (format == FIF_JPEG) {
            flags = JPEG_ACCURATE;
        } else if (format == FIF_TARGA) {
            flags = TARGA_LOAD_RGB888;
        }

        auto bitmap = FreeImage_Load(format, filename.c_str(), flags);
        auto bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
        auto width = FreeImage_GetWidth(bitmap32);
        auto height = FreeImage_GetHeight(bitmap32);
        auto redMask = FreeImage_GetRedMask(bitmap32);
        auto greenMask = FreeImage_GetGreenMask(bitmap32);
        auto blueMask = FreeImage_GetBlueMask(bitmap32);
        void* data = FreeImage_GetBits(bitmap32);
        GLenum fmt = GL_RGBA;
        if (redMask > greenMask && greenMask > blueMask) fmt = GL_BGRA;
        auto internalFmt = GL_RGBA8;
        for (unsigned int i = 1; i < fileOptions.size(); ++i) {
            boost::trim(fileOptions[i]);
            if (fileOptions[i] == "sRGB" && application->GetConfig().useSRGB) internalFmt = GL_SRGB8_ALPHA8;
        }
        TextureDescriptor texDesc(4, internalFmt, fmt, GL_UNSIGNED_BYTE);
        texture = std::make_unique<GLTexture>(width, height, texDesc, data);
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
