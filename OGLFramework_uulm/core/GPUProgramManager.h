/**
 * @file   GPUProgramManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the definition of GPUProgramManager.
 */

#ifndef GPUPROGRAMMANAGER_H
#define GPUPROGRAMMANAGER_H

#include "gfx/glrenderer/GPUProgram.h"

/** ResourceManager interface for GPUProgram resources. */
typedef IResourceManager<GPUProgram> IGPUProgramManager;

/**
 * @brief  Manages the GPUProgram resources.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 */
class GPUProgramManager : public IGPUProgramManager
{
private:
    /** Deleted copy constructor. */
    GPUProgramManager(const GPUProgramManager&) {};
    /** Deleted copy assignment operator. */
    GPUProgramManager& operator=(const GPUProgramManager&) { return *this; };

public:
    /** The gpu program type. */
    typedef IGPUProgramManager::ResourceType GPUProgramType;
    /** The gpu program map type. */
    typedef IGPUProgramManager::ResourceMap GPUProgramMap;

    GPUProgramManager(ApplicationBase* app);
    virtual ~GPUProgramManager() {};

    GPUProgramType* GetResource(const std::string& resId) override;
    bool HasResource(const std::string& resId) override;
    void RecompileAll();

private:
    /** Holds the GPUProgram objects. */
    GPUProgramMap gpuPrograms;
    /** Holds the window for dependencies. */
    ApplicationBase* application;

    void HandleShaderCompileException(const shader_compiler_error& except);
};

#endif /* GPUPROGRAMMANAGER_H */
