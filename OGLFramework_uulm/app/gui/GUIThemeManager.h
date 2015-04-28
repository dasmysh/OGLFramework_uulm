/**
 * @file   GUIThemeManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the definition of GUIThemeManager.
 */

#ifndef GUITHEMEMANAGER_H
#define GUITHEMEMANAGER_H

#include "GUITheme.h"

class ApplicationBase;

/** ResourceManager interface for GUITheme resources. */
typedef IResourceManager<GUITheme> IGUIThemeManager;

/**
 * @brief  Contains the ResourceManager implementation for GUIThemes.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 */
class GUIThemeManager : public IGUIThemeManager
{
public:
    /** The font type. */
    typedef IGUIThemeManager::ResourceType GUIThemeType;
    /** The font pointer utility type. */
    typedef IGUIThemeManager::ResourcePtrUtil GUIThemePtrUtil;
    /** The font map type. */
    typedef IGUIThemeManager::ResourceMap GUIThemeMap;

    GUIThemeManager(ApplicationBase* app);
    virtual ~GUIThemeManager() = default;
    /** Deleted copy constructor. */
    GUIThemeManager(const GUIThemeManager& orig) = delete;
    /** Deleted copy assignment operator. */
    GUIThemeManager& operator=(const GUIThemeManager& orig) = delete;
    /** Default move constructor. */
    GUIThemeManager(GUIThemeManager&& orig) = default;
    /** Default move assignment operator. */
    GUIThemeManager& operator=(GUIThemeManager&& orig) = default;

    typename GUIThemePtrUtil::SharedTPtr GetResource(const std::string& resId) override;
    bool HasResource(const std::string& resId) override;

private:
    /** Holds the Font objects. */
    GUIThemeMap themes = {};
    /** Holds the application object for dependencies. */
    ApplicationBase* application;

};

#endif /* GUITHEMEMANAGER_H */
