/**
 * @file   TextureManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.24
 *
 * @brief  Contains the definition of TextureManager.
 */

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "../gfx/glrenderer/GLTexture2D.h"

namespace cgu {

    /**
     * @brief  ResourceManager implementation for GLTexture2D resources.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.02.24
     */
    class TextureManager final : public ResourceManager<GLTexture2D>
    {
    public:
        explicit TextureManager(ApplicationBase* app);
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager(TextureManager&&);
        TextureManager& operator=(TextureManager&&);
        virtual ~TextureManager();
    };
}

#endif /* TEXTUREMANAGER_H */
