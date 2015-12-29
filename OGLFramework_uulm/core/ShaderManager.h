/**
 * @file   ShaderManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.15
 *
 * @brief  Contains the definition of ShaderManager.
 */

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "gfx/glrenderer/Shader.h"

namespace cgu {

    /**
     * @brief  Manages Shader resources.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.15
     */
    class ShaderManager : public ResourceManager<Shader>
    {
    public:
        explicit ShaderManager(ApplicationBase* app);
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;
        ShaderManager(ShaderManager&&);
        ShaderManager& operator=(ShaderManager&&);
        virtual ~ShaderManager();

    protected:
        void LoadResource(const std::string& resId, ResourceType* resourcePtr) override;
    };
}

#endif /* SHADERMANAGER_H */
