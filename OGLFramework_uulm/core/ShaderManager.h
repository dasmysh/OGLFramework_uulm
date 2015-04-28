/**
 * @file   ShaderManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the definition of ShaderManager.
 */

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "gfx/glrenderer/Shader.h"

/** ResourceManager interface for shader resources. */
typedef IResourceManager<Shader> IShaderManager;

/**
 * @brief  Manages Shader resources.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 */
class ShaderManager : public IShaderManager
{
private:
    /** Deleted copy constructor. */
    ShaderManager(const ShaderManager&) {};
    /** Deleted copy assignment operator. */
    ShaderManager& operator=(const ShaderManager&) {};

public:
    /** The material library type. */
    typedef IShaderManager::ResourceType ShaderType;
    /** The material library map type. */
    typedef IShaderManager::ResourceMap ShaderMap;

    ShaderManager(ApplicationBase* app);
    virtual ~ShaderManager() {};

    ShaderType* GetResource(const std::string& resId) override;
    bool HasResource(const std::string& resId) override;

private:
    /** Holds the loaded shaders. */
    ShaderMap shaders;
    /** Holds the window for dependencies. */
    ApplicationBase* application;
};

#endif /* SHADERMANAGER_H */
