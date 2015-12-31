/**
 * @file   TextureManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.24
 *
 * @brief  Contains the implementation of TextureManager.
 */

#include "TextureManager.h"

namespace cgu {

    /**
     * Constructor.
     * @param app the application object for resolving dependencies.
     */
    TextureManager::TextureManager(ApplicationBase* app) :
        ResourceManagerBase(app)
    {
    }

    /** Default copy constructor. */
    TextureManager::TextureManager(const TextureManager&) = default;
    /** Default copy assignment operator. */
    TextureManager& TextureManager::operator=(const TextureManager&) = default;

    /** Default move constructor. */
    TextureManager::TextureManager(TextureManager&& rhs) : ResourceManagerBase(std::move(rhs)) {}

    /** Default move assignment operator. */
    TextureManager& TextureManager::operator=(TextureManager&& rhs)
    {
        ResourceManagerBase* tResMan = this;
        *tResMan = static_cast<ResourceManagerBase&&>(std::move(rhs));
        return *this;
    }

    /** Default destructor. */
    TextureManager::~TextureManager() = default;
}
