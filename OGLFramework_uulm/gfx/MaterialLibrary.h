/**
 * @file   MaterialLibrary.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   4. Januar 2014
 *
 * @brief  Contains definition of MaterialLibrary.
 */

#ifndef MATERIALLIBRARY_H
#define MATERIALLIBRARY_H

#include <boost/regex.hpp>

#include "../main.h"
#include "Material.h"

namespace cgu {
    class GLTexture2D;

    /**
     * @brief IResourceManager for Material resources.
     */
    typedef IResourceManager<Material> IMaterialManager;

    /**
     * @brief  Loads material libraries and manages its materials.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   4. Januar 2014
     */
    class MaterialLibrary : public Resource, IMaterialManager
    {
    private:
        /** Copy constructor deleted. */
        MaterialLibrary(const MaterialLibrary& orig) : Resource(orig) {};
        /** Copy assignment operator deleted. */
        MaterialLibrary& operator=(const MaterialLibrary&) {};

    public:
        /** The material type. */
        typedef IMaterialManager::ResourceType MaterialType;
        /** The material map type. */
        typedef IMaterialManager::ResourceMap MaterialMap;

        MaterialLibrary(const std::string& mtlFilename, ApplicationBase* app);
        virtual ~MaterialLibrary();

        virtual void Load() override;
        virtual void Unload() override;

        MaterialType* GetResource(const std::string& resId) override;
        bool HasResource(const std::string& resId) override;

    private:
        /** Holds the loaded materials. */
        MaterialMap materials;

        void UnloadLocal();
        glm::vec3 parseColor(const boost::smatch& matches) const;
        const GLTexture2D* parseTexture(const std::string& matches, const std::string& params) const;
        float parseFloatParameter(const std::string& paramName, const std::string& matches, float defaultValue) const;
        void notImplemented(const std::string& feature) const;
    };
}

#endif /* MATERIALLIBRARY_H */
