/**
 * @file   MaterialLibManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.06
 *
 * @brief  Contains the definition of the material manager.
 */

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "gfx/MaterialLibrary.h"

namespace cgu {

    class ApplicationBase;

    /**
     * @brief  ResourceManager implementation for Material objects.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.06
     */
    class MaterialLibManager final : public ResourceManager<MaterialLibrary>
    {
    public:
        explicit MaterialLibManager(ApplicationBase* app);
        MaterialLibManager(const MaterialLibManager&);
        MaterialLibManager& operator=(const MaterialLibManager&);
        MaterialLibManager(MaterialLibManager&&);
        MaterialLibManager& operator=(MaterialLibManager&&);
        virtual ~MaterialLibManager();
    };
}

#endif /* MATERIALMANAGER_H */
