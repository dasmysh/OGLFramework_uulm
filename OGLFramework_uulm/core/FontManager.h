/**
 * @file   FontManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.24
 *
 * @brief  Contains the definition of FontManager.
 */

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "gfx/glrenderer/Font.h"

namespace cgu {

    /**
     * @brief  ResourceManager implementation for font resources.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.02.24
     */
    class FontManager final : public ResourceManager<Font>
    {
    public:
        explicit FontManager(ApplicationBase* app);
        FontManager(const FontManager&) = delete;
        FontManager& operator=(const FontManager&) = delete;
        FontManager(FontManager&&);
        FontManager& operator=(FontManager&&);
        virtual ~FontManager();
    };
}

#endif /* FONTMANAGER_H */
