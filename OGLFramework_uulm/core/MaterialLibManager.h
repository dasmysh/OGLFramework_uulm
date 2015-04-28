/**
 * @file   MaterialLibManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   6. Januar 2014
 *
 * @brief  Contains the definition of the material manager.
 */

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "gfx/MaterialLibrary.h"

/**
 * @brief IResourceManager for MaterialLibrary resources.
 */
typedef IResourceManager<MaterialLibrary> IMaterialLibManager;

/**
 * @brief  ResourceManager implementation for Material objects.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   6. Januar 2014
 */
class MaterialLibManager final : public IMaterialLibManager
{
private:
    /** Copy constructor deleted. */
    MaterialLibManager(const MaterialLibManager&) {};
    /** Copy assignment operator deleted. */
    MaterialLibManager& operator=(const MaterialLibManager&) {};
public:
    /** The material library type. */
    typedef IMaterialLibManager::ResourceType MaterialLibType;
    /** The material library map type. */
    typedef IMaterialLibManager::ResourceMap MaterialLibMap;

    MaterialLibManager(ApplicationBase* app);
    virtual ~MaterialLibManager() {};

    MaterialLibType* GetResource(const std::string& resId) override;
    bool HasResource(const std::string& resId) override;

private:
    /** Holds the application object. */
    ApplicationBase* application;
    /** Holds the loaded material libraries. */
    MaterialLibMap materialLibs;
};

#endif /* MATERIALMANAGER_H */
