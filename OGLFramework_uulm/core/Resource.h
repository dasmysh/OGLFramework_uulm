/**
 * @file   Resource.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2013.12.31
 *
 * @brief  Contains Resource, a base class for all managed resources.
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "main.h"

namespace cgu {

    class ApplicationBase;

    /**
     * @brief  Base class for all managed resources.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2013.12.31
     */
    class Resource
    {
    public:
        Resource(const std::string& resourceId, ApplicationBase* app);
        Resource(const Resource&);
        Resource& operator=(const Resource&);
        Resource(Resource&&);
        Resource& operator=(Resource&&);
        virtual ~Resource();

        const std::string& getId() const;
        virtual void Load();
        virtual void Unload();
        bool IsLoaded() const;

    protected:
        /** A list of sub-resources. */
        using SubResourceList = std::vector<std::string>;
        /** A list of parameters. */
        using ParameterList = std::vector<std::string>;

        static std::string GetNormalizedResourceId(const std::string& resId);
        SubResourceList GetSubresources() const;
        ParameterList GetParameters() const;

        /** Holds the resources id. */
        std::string id;
        /** Holds the application object for dependencies. */
        ApplicationBase* application;
        /** Holds if the resource is currently loaded. */
        bool loaded;
    };
}

#endif /* RESOURCE_H */
