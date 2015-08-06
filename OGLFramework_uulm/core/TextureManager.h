/**
 * @file   TextureManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the definition of TextureManager.
 */

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "../gfx/glrenderer/GLTexture2D.h"

namespace cgu {
    /** IResourceManager for GLTexture2D resources. */
    typedef IResourceManager<GLTexture2D> ITextureManager;

    /**
     * @brief  ResourceManager implementation for GLTexture2D resources.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   24. Februar 2014
     */
    class TextureManager : public ITextureManager
    {
    private:
        /** Deleted copy constructor. */
        TextureManager(const TextureManager&) = delete;
        /** Deleted copy assignment operator. */
        TextureManager& operator=(const TextureManager&) = delete;

    public:
        /** The font type. */
        typedef ITextureManager::ResourceType TextureType;
        /** The font map type. */
        typedef ITextureManager::ResourceMap TextureMap;

        TextureManager(ApplicationBase* app);
        virtual ~TextureManager();

        TextureType* GetResource(const std::string& resId) override;
        bool HasResource(const std::string& resId) override;

    private:
        /** Holds the GLTexture2D objects. */
        TextureMap textures;
        /** Holds the application object for dependencies. */
        ApplicationBase* application;
    };
}

#endif /* TEXTUREMANAGER_H */
