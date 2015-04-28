/**
 * @file   ResourceManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   3. Januar 2014
 *
 * @brief  Contains the base class for all resource managers.
 */

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "main.h"

/**
 * @brief Interface for all resource managers.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   3. Januar 2014
 */
template<typename rType>
class IResourceManager
{
public:
    /** The resource managers resource type. */
    typedef rType ResourceType;
    /** The resource map type. */
    typedef typename std::unordered_map<std::string, std::unique_ptr<rType>> ResourceMap;

    virtual ~IResourceManager() {};
    /**
     * Gets a resource from the manager.
     * @param resId the resources id
     * @return the resource as a shared pointer
     */
    virtual ResourceType* GetResource(const std::string& resId) = 0;

    /**
     * Checks if the resource manager contains this resource (needed for some managers which are
     * not <em>singletons</em> like the MaterialLibrary objects).
     * @param resId the resources id
     * @return whether the manager contains the resource or not.
     */
    virtual bool HasResource(const std::string& resId) = 0;
};

/**
 * @brief  Base class for all resource managers.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   3. Januar 2014
 */
template<typename rType>
class ResourceManager : public IResourceManager<rType>
{
private:
    /** Copy constructor deleted. */
    ResourceManager(const ResourceManager&) {};
    /** Copy assignment operator deleted. */
    ResourceManager& operator=(const ResourceManager&) { return *this; };

public:
    /** The resource managers resource type. */
    typedef typename IResourceManager<rType>::ResourceType ResourceType;
    /** The resource map type. */
    typedef typename IResourceManager<rType>::ResourceMap ResourceMap;

    ResourceManager() : resources() {};
    virtual ~ResourceManager() {};

    /** @see IResourceManager::GetResource */
    ResourceType* GetResource(const std::string& resId) override
    {
        try {
            return ResourcePtrUtil::share(resources.at(resId));
        } catch (std::out_of_range e) {
            LOG(Log::LogLevel::LL_INFO) << L"No resource with id \"" << std::wstring(resId.begin(), resId.end())
                    << L"\" found. Creating new one.";
            return AddNewResource(resId);
        }
    }

    /** @see IResourceManager::HasResource */
    virtual bool HasResource(const std::string& resId) override
    {
        typename ResourceMap::const_iterator got = resources.find(resId);
        if (got == resources.end())
            return false;
        return true;
    }


protected:

    /**
     * Adds a new resource to the manager and loads it.
     * @param resId the resources id
     * @return the resource as a shared pointer
     */
    virtual typename ResourceType* AddNewResource(const std::string& resId)
    {
        typename ResourcePtrUtil::TPtr res = ResourcePtrUtil::template make(resId);
        typename ResourcePtrUtil::SharedTPtr resShared = ResourcePtrUtil::share(res);
        res->Load();
        InsertResource(resId, res);
        return resShared;
    }

    /**
     * Inserts a new resource into the resource map.
     * @param resId the resources id
     * @param res the resource pointer
     */
    void InsertResource(const std::string& resId, std::unique_ptr<rType>& res)
    {
        resources.insert(std::make_pair(resId, std::move(res)));
    }

private:
    /** Holds the resources managed. */
    ResourceMap resources;
};

#endif /* RESOURCEMANAGER_H */
