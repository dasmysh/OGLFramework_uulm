/**
 * @file   FontManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the implementation of FontManager.
 */

#include "FontManager.h"
#include "app/ApplicationBase.h"
#include "gfx/glrenderer/GLTexture.h"
#include "gfx/glrenderer/GLUniformBuffer.h"

namespace cgu {

    FontManager::FontManager(ApplicationBase* app) :
        application(app),
        fonts()
    {
    }

    FontManager::~FontManager()
    {
    }

    FontManager::FontType* FontManager::GetResource(const std::string& resId)
    {
        try {
            return fonts.at(resId).get();
        }
        catch (std::out_of_range e) {
            LOG(INFO) << L"No resource with id \"" << resId << L"\" found. Creating new one.";
            std::unique_ptr<FontType> fontPtr(new FontType(resId, application));
            FontType* fontRawPtr = fontPtr.get();
            fonts.insert(std::make_pair(resId, std::move(fontPtr)));
            return fontRawPtr;
        }
    }

    bool FontManager::HasResource(const std::string& resId)
    {
        auto got = fonts.find(resId);
        if (got == fonts.end())
            return false;
        return true;
    }
}
