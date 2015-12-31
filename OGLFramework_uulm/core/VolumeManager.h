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

    /**
    * @brief  ResourceManager implementation for GLTexture3D resources.
    *
    * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
    * @date   2015.08.16
    */
    class VolumeManager final : public ResourceManager<GLTexture3D>
    {
    public:
        explicit VolumeManager(ApplicationBase* app);
        VolumeManager(const VolumeManager&);
        VolumeManager& operator=(const VolumeManager&);
        VolumeManager(VolumeManager&&);
        VolumeManager& operator=(VolumeManager&&);
        virtual ~VolumeManager();
    };
}

#endif // VOLUMEMANAGER_H
