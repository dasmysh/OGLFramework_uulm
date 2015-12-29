/**
 * @file   MaterialLibManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.06
 *
 * @brief  Contains the implementation of the material manager.
 */

#include "MaterialLibManager.h"

namespace cgu {

    /**
     * Constructor.
     * @param app the application object for resolving dependencies.
     */
    MaterialLibManager::MaterialLibManager(ApplicationBase* app) :
        ResourceManagerBase(app)
    {
    }

    /** Default move constructor. */
    MaterialLibManager::MaterialLibManager(MaterialLibManager&& rhs) : ResourceManagerBase(std::move(rhs)) {}

    /** Default move assignment operator. */
    MaterialLibManager& MaterialLibManager::operator=(MaterialLibManager&& rhs)
    {
        ResourceManagerBase* tResMan = this;
        *tResMan = static_cast<ResourceManagerBase&&>(std::move(rhs));
        return *this;
    }

    /** Default destructor. */
    MaterialLibManager::~MaterialLibManager() = default;
}
