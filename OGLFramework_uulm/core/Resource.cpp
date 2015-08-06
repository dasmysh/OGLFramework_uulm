/**
* @file    Resource.cpp
* @author  Sebastian Maisch <sebastian.maisch@googlemail.com>
* @date    2014.03.29
*
* @brief   Implementations for the resource class.
*/

#include "Resource.h"

namespace cgu {
    /**
     * Constructor.
     * @param resourceId the resource id to use
     * @param app the application object for dependencies
     */
    Resource::Resource(const std::string& resourceId, ApplicationBase* app) :
        id(resourceId),
        application(app),
        loaded(false)
    {
    };

    /** Default copy constructor. */
    Resource::Resource(const Resource& orig) :
        id(orig.id),
        application(orig.application)
    {
        if (orig.loaded) Load();
    }

    /** Default assignment operator. */
    Resource& Resource::operator=(const Resource& orig)
    {
        id = orig.id;
        application = orig.application;
        return *this;
    }

    /** Move constructor. */
    Resource::Resource(Resource&& orig) :
        id(std::move(orig.id)),
        application(std::move(orig.application)),
        loaded(std::move(orig.loaded))
    {
        orig.loaded = false;
        orig.application = nullptr;
    };

    /** Move assignment operator. */
    Resource& Resource::operator=(Resource&& orig)
    {
        if (this != &orig) {
            if (loaded) Unload();
            id = std::move(orig.id);
            application = orig.application;
            loaded = orig.loaded;
            orig.loaded = false;
            orig.application = nullptr;
        }
        return *this;
    };

    Resource::~Resource()
    {
        if (loaded) this->Unload();
    };

    /**
    * Accessor to the resources id.
    * @return the resources id
    */
    const std::string& Resource::getId() const
    {
        return id;
    };

    /**
    * @brief Loads a resource. Call this method in derived classes Load method last.
    */
    void Resource::Load()
    {
        loaded = true;
    };

    /**
    * @brief Unloads a resource. Call this method in derived classes Unload method last.
    * This method should free all resources or memory obtained by the Load method.
    */
    void Resource::Unload()
    {
        loaded = false;
    };

    /**
    * Checks if the resource is loaded.
    * @return <code>true</code> if resource is currently loaded
    */
    bool Resource::IsLoaded()
    {
        return loaded;
    }
}