/**
 * @file   Resource.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 *
 * @brief  Contains Resource, a base class for all managed resources.
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "main.h"

class ApplicationBase;

/**
 * @brief  Base class for all managed resources.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 */
class Resource
{
public:
    Resource(const std::string& resourceId, ApplicationBase* app);
    Resource(const Resource& orig);
    Resource& operator=(const Resource& orig);
    Resource(Resource&& orig);
    Resource& operator=(Resource&& orig);
    virtual ~Resource();

    const std::string& getId() const;
    virtual void Load();
    virtual void Unload();
    bool IsLoaded();

protected:
    /** Holds the resources id. */
    std::string id;
    /** Holds the application object for dependencies. */
    ApplicationBase* application;
    /** Holds if the resource is currently loaded. */
    bool loaded;
};

#endif /* RESOURCE_H */
