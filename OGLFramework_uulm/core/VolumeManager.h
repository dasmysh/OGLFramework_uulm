/**
 * @file   VolumeManager.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.16
 *
 * @brief  Contains the definition of VolumeManager.
 */

#ifndef VOLUMEMANAGER_H
#define VOLUMEMANAGER_H

#include "gfx/glrenderer/GLTexture3D.h"

namespace cgu {
    /** IResourceManager for GLTexture3D resources. */
    typedef IResourceManager<GLTexture3D> IVolumeManager;

    /**
    * @brief  ResourceManager implementation for GLTexture3D resources.
    *
    * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
    * @date   2015.08.16
    */
    class VolumeManager : public IVolumeManager
    {
    public:
        /** The font type. */
        typedef VolumeManager::ResourceType VolumeType;
        /** The font map type. */
        typedef VolumeManager::ResourceMap VolumeMap;

        VolumeManager(ApplicationBase* app);
        VolumeManager(const VolumeManager&) = delete;
        VolumeManager& operator=(const VolumeManager&) = delete;
        virtual ~VolumeManager();

        VolumeType* GetResource(const std::string& resId) override;
        bool HasResource(const std::string& resId) override;

    private:
        /** Holds the GLTexture3D objects. */
        VolumeMap volumes;
        /** Holds the application object for dependencies. */
        ApplicationBase* application;
    };
}

#endif // VOLUMEMANAGER_H
