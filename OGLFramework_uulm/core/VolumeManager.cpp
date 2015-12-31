/**
 * @file   VolumeManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.16
 *
 * @brief  
 */

#include "VolumeManager.h"

namespace cgu {

    /**
     * Constructor.
     * @param app the application object for resolving dependencies.
     */
    VolumeManager::VolumeManager(ApplicationBase* app) :
        ResourceManagerBase(app)
    {
    }

    /** Default copy constructor. */
    VolumeManager::VolumeManager(const VolumeManager&) = default;
    /** Default copy assignment operator. */
    VolumeManager& VolumeManager::operator=(const VolumeManager&) = default;

    /** Default move constructor. */
    VolumeManager::VolumeManager(VolumeManager&& rhs) : ResourceManagerBase(std::move(rhs)) {}

    /** Default move assignment operator. */
    VolumeManager& VolumeManager::operator=(VolumeManager&& rhs)
    {
        ResourceManagerBase* tResMan = this;
        *tResMan = static_cast<ResourceManagerBase&&>(std::move(rhs));
        return *this;
    }

    /** Default destructor. */
    VolumeManager::~VolumeManager() = default;
}
