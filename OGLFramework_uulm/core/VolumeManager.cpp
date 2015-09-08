/**
 * @file   VolumeManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.16
 *
 * @brief  
 */

#include "VolumeManager.h"

namespace cgu {

    VolumeManager::VolumeManager(ApplicationBase* app) :
        volumes(),
        application(app)
    {
    }

    VolumeManager::~VolumeManager()
    {
    }

    VolumeManager::VolumeType* VolumeManager::GetResource(const std::string& resId)
    {
        try {
            return volumes.at(resId).get();
        }
        catch (std::out_of_range e) {
            LOG(INFO) << L"No resource with id \"" << resId << L"\" found. Creating new one.";
            std::unique_ptr<VolumeType> volumePtr(new VolumeType(resId, application));
            VolumeType* volumeRawPtr = volumePtr.get();
            volumeRawPtr->Load();
            volumes.insert(std::make_pair(resId, std::move(volumePtr)));
            return volumeRawPtr;
        }
    }

    bool VolumeManager::HasResource(const std::string & resId)
    {
        auto got = volumes.find(resId);
        if (got == volumes.end())
            return false;
        return true;
    }
}
