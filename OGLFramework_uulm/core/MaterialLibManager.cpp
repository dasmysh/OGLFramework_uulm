/**
 * @file   MaterialLibManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   6. Januar 2014
 *
 * @brief  Contains the implemenetation of the material manager.
 */

#include "MaterialLibManager.h"

namespace cgu {

    /**
     * Constructor.
     * @param texMan the texture manager to use
     */
    MaterialLibManager::MaterialLibManager(ApplicationBase* app) :
        application(app),
        materialLibs()
    {
    }

    MaterialLibManager::~MaterialLibManager()
    {
    }

    MaterialLibManager::MaterialLibType* MaterialLibManager::GetResource(const std::string& resId)
    {
        try {
            return materialLibs.at(resId).get();
        }
        catch (std::out_of_range e) {
            LOG(INFO) << L"No resource with id \"" << resId << L"\" found. Creating new one.";
            std::unique_ptr<MaterialLibType> matLibPtr(new MaterialLibType(resId, application));
            MaterialLibType* matLibRawPtr = matLibPtr.get();
            matLibPtr->Load();
            materialLibs.insert(std::make_pair(resId, std::move(matLibPtr)));
            return matLibRawPtr;
        }
    }

    bool MaterialLibManager::HasResource(const std::string& resId)
    {
        auto got = materialLibs.find(resId);
        if (got == materialLibs.end())
            return false;
        return true;
    }
}
