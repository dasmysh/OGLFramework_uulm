/**
 * @file   FontManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.24
 *
 * @brief  Contains the implementation of FontManager.
 */

#include "FontManager.h"
#include "app/ApplicationBase.h"

namespace cgu {

    /**
     * Constructor.
     * @param app the application object for resolving dependencies.
     */
    FontManager::FontManager(ApplicationBase* app) :
        ResourceManagerBase(app)
    {
    }

    /** Default move constructor. */
    FontManager::FontManager(FontManager&& rhs) : ResourceManagerBase(std::move(rhs)) {}

    /** Default move assignment operator. */
    FontManager& FontManager::operator=(FontManager&& rhs)
    {
        ResourceManagerBase* tResMan = this;
        *tResMan = static_cast<ResourceManagerBase&&>(std::move(rhs));
        return *this;
    }

    /** Default destructor. */
    FontManager::~FontManager() = default;
}
