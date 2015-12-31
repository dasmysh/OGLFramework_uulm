/**
 * @file   ResourceManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.03
 *
 * @brief  Contains the base class for all resource managers.
 */

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <boost/exception/all.hpp>
#include <codecvt>

namespace cgu {

    class ApplicationBase;

    using errdesc_info = boost::error_info<struct tag_errdesc, std::string>;
    using resid_info = boost::error_info<struct tag_resid, std::string>;

    /**
     * Exception base class for resource loading errors.
     */
    struct resource_loading_error : virtual boost::exception, virtual std::exception { };

    template<typename rType>
    struct DefaultResourceLoadingPolicy
    {
        static std::unique_ptr<rType> CreateResource(const std::string& resId, ApplicationBase* app)
        {
            return std::move(std::make_unique<rType>(resId, app));
        }

        static bool IsResourceLoaded(const rType* res) { return res->IsLoaded(); }
        static void LoadResource(rType* res) { return res->Load(); }
    };

    /**
     * @brief  Base class for all resource managers.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.03
     */
    template<typename rType, bool reloadLoop = false, typename ResourceLoadingPolicy = DefaultResourceLoadingPolicy<rType>>
    class ResourceManager
    {
    protected:
        /** The resource managers resource type. */
        using ResourceType = rType;
        /** The resource map type. */
        using ResourceMap = std::unordered_map<std::string, std::unique_ptr<rType>>;
        /** The type of this base class. */
        using ResourceManagerBase = ResourceManager<rType, reloadLoop, ResourceLoadingPolicy>;

    public:
        /** Constructor for resource managers. */
        explicit ResourceManager(ApplicationBase* app) : application{ app } {};

        /** Copy constructor. */
        ResourceManager(const ResourceManager& rhs) : ResourceManager(rhs.application)
        {
            for (const auto& res : rhs.resources) {
                std::unique_ptr<ResourceType> resourcePtr = std::move(ResourceLoadingPolicy::CreateResource(res.first, application));
                ResourceLoadingPolicy::LoadResource(resourcePtr.get());
                resources.insert(std::move(std::make_pair(res.first, std::move(resourcePtr))));
            }
        }

        /** Copy assignment operator. */
        ResourceManager& operator=(const ResourceManager& rhs)
        {
            ResourceManager tmp{ rhs };
            std::swap(application, tmp.application);
            std::swap(resources, tmp.resources);
            return *this;
        }

        /** Default move constructor. */
        ResourceManager(ResourceManager&& rhs) : resources(std::move(rhs.resources)), application(rhs.application) {}
        /** Default move assignment operator. */
        ResourceManager& operator=(ResourceManager&& rhs)
        {
            if (this != &rhs) {
                resources = std::move(rhs.resources);
                application = rhs.application;
                rhs.application = nullptr;
            }
            return *this;
        };
        /** Default destructor. */
        virtual ~ResourceManager() = default;

        /**
         * Gets a resource from the manager.
         * @param resId the resources id
         * @return the resource as a shared pointer
         */
        ResourceType* GetResource(const std::string& resId)
        {
            try {
                return resources.at(resId).get();
            }
            catch (std::out_of_range e) {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                LOG(INFO) << L"No resource with id \"" << converter.from_bytes(resId) << L"\" found. Creating new one.";
                std::unique_ptr<ResourceType> resourcePtr = std::move(ResourceLoadingPolicy::CreateResource(resId, application));
                //(new ResourceType(resId, application));
                ResourceType* resourceRawPtr = resourcePtr.get();
                LoadResource(resId, resourceRawPtr);
                while (reloadLoop && !ResourceLoadingPolicy::IsResourceLoaded(resourceRawPtr)) {
                    LoadResource(resId, resourceRawPtr);
                }
                resources.insert(std::move(std::make_pair(resId, std::move(resourcePtr))));
                return resourceRawPtr;
            }
        }

        /**
         * Checks if the resource manager contains this resource (needed for some managers which are
         * not <em>singletons</em> like the MaterialLibrary objects).
         * @param resId the resources id
         * @return whether the manager contains the resource or not.
         */
        bool HasResource(const std::string& resId) const
        {
            return (resources.find(resId) != resources.end());
        }


    protected:
        /**
         * Loads a new resource and handles errors.
         * @param resourcePtr pointer to the resource.
         */
        virtual void LoadResource(const std::string& resId, ResourceType* resourcePtr)
        {
            try {
                ResourceLoadingPolicy::LoadResource(resourcePtr);
            }
            catch (const resource_loading_error& loadingError) {
                auto resid = boost::get_error_info<resid_info>(loadingError);
                auto filename = boost::get_error_info<boost::errinfo_file_name>(loadingError);
                auto errDesc = boost::get_error_info<errdesc_info>(loadingError);
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                LOG(INFO) << L"Error while loading resource \"" << converter.from_bytes(resId) << L"\"/\"" << resId.c_str() << "\"." << std::endl
                    << "ResourceID: " << (resid == nullptr ? "-" : resid->c_str()) << std::endl
                    << "Filename: " << (filename == nullptr ? "-" : filename->c_str()) << std::endl
                    << "Description: " << (errDesc == nullptr ? "-" : errDesc->c_str());
                if (!reloadLoop) throw;
            }
        }

        /**
         *  Sets the resource with a given name to a new value.
         *  @param resourceName the name of the resource.
         *  @param resource the new resource.
         *  @return a pointer to the new resource.
         */
        ResourceType* SetResource(const std::string& resourceName, std::unique_ptr<ResourceType> resource)
        {
            resources[resourceName] = std::move(resource);
            return resources[resourceName].get();
        }

        /** Holds the resources managed. */
        ResourceMap resources;
        /** Holds the application base. */
        ApplicationBase* application;
    };
}

#endif /* RESOURCEMANAGER_H */
