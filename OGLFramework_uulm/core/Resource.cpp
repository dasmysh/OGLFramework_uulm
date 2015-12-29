/**
* @file    Resource.cpp
* @author  Sebastian Maisch <sebastian.maisch@googlemail.com>
* @date    2014.03.29
*
* @brief   Implementations for the resource class.
*/

#include "Resource.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace cgu {
    /**
     * Constructor.
     * @param resourceId the resource id to use
     * @param app the application object for dependencies
     */
    Resource::Resource(const std::string& resourceId, ApplicationBase* app) :
        id(GetNormalizedResourceId(resourceId)),
        application(app),
        loaded(false)
    {
    };

    /** Default copy constructor. */
    Resource::Resource(const Resource&) = default;

    /** Default assignment operator. */
    Resource& Resource::operator=(const Resource&) = default;

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

    Resource::~Resource() = default;

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
    bool Resource::IsLoaded() const
    {
        return loaded;
    }

    /**
     *  Returns the normalized resource id (no global parameters).
     *  @param the resource id.
     *  @return the normalized resource id.
     */
    std::string Resource::GetNormalizedResourceId(const std::string& resId)
    {
        std::vector<std::string> subresourcesAndGlobalParams;
        boost::split_regex(subresourcesAndGlobalParams, resId, boost::regex("\\|,"));
        for (auto& sr : subresourcesAndGlobalParams) boost::trim(sr);

        SubResourceList subresources;
        boost::split(subresources, subresourcesAndGlobalParams[0], boost::is_any_of("|"));
        for (auto& sr : subresources) {
            boost::trim(sr);
            if (subresourcesAndGlobalParams.size() > 1) sr += "," + subresourcesAndGlobalParams[1];
        }
        return boost::join(subresources, "|");
    }

    /**
     *  Returns the list of sub-resources.
     *  @return a list of sub-resource ids.
     */
    Resource::SubResourceList Resource::GetSubresources() const
    {
        SubResourceList subresources;
        boost::split(subresources, id, boost::is_any_of("|"));
        for (auto& sr : subresources) boost::trim(sr);
        return subresources;
    }

    /**
     *  Returns a list of parameters for this resource.
     *  @return a list of parameter strings.
     */
    Resource::ParameterList Resource::GetParameters() const
    {
        assert(GetSubresources().size() == 1);
        ParameterList parameters;
        boost::split(parameters, id, boost::is_any_of(","));
        for (auto& param : parameters) boost::trim(param);
        return parameters;
    }
}
