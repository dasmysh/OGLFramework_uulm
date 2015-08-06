/**
 * @file   FontManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the definition of FontManager.
 */

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "gfx/glrenderer/Font.h"

namespace cgu {

    class ApplicationBase;

    /** ResourceManager interface for Font resources. */
    typedef IResourceManager<Font> IFontManager;

    /**
     * @brief  ResourceManager implementation for font resources.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   24. Februar 2014
     */
    class FontManager : public IFontManager
    {
    private:
        /** Deleted copy constructor. */
        FontManager(const FontManager&) {};
        /** Deleted copy assignment operator. */
        FontManager& operator=(const FontManager&) {};

    public:
        /** The font type. */
        typedef IFontManager::ResourceType FontType;
        /** The font map type. */
        typedef IFontManager::ResourceMap FontMap;

        FontManager(ApplicationBase* app);
        virtual ~FontManager();

        FontType* GetResource(const std::string& resId) override;
        bool HasResource(const std::string& resId) override;

    private:
        /** Holds the Font objects. */
        FontMap fonts;
        /** Holds the application object for dependencies. */
        ApplicationBase* application;

    };
}

#endif /* FONTMANAGER_H */
