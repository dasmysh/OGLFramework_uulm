/**
 * @file   TextureManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the implementation of TextureManager.
 */

#include "TextureManager.h"
#include "gfx/glrenderer/GLTexture.h"

namespace cgu {

    TextureManager::TextureManager(ApplicationBase* app) :
        textures(),
        application(app)
    {
    }

    TextureManager::~TextureManager()
    {
    }

    TextureManager::TextureType* TextureManager::GetResource(const std::string& resId)
    {
        try {
            return textures.at(resId).get();
        }
        catch (std::out_of_range e) {
            LOG(INFO) << L"No resource with id \"" << resId << L"\" found. Creating new one.";
            std::unique_ptr<TextureType> texturePtr(new TextureType(resId, application));
            TextureType* textureRawPtr = texturePtr.get();
            textures.insert(std::make_pair(resId, std::move(texturePtr)));
            return textureRawPtr;
        }
    }

    bool TextureManager::HasResource(const std::string & resId)
    {
        auto got = textures.find(resId);
        if (got == textures.end())
            return false;
        return true;
    }
}
