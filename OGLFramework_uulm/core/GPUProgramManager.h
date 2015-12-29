/**
 * @file   GPUProgramManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.15
 * @brief  Contains the definition of GPUProgramManager.
 */

#ifndef GPUPROGRAMMANAGER_H
#define GPUPROGRAMMANAGER_H

#include "gfx/glrenderer/GPUProgram.h"

namespace cgu {

    struct shader_compiler_error;

    /**
     * @brief  Manages the GPUProgram resources.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.15
     */
    class GPUProgramManager : public ResourceManager<GPUProgram, true>
    {
    public:
        explicit GPUProgramManager(ApplicationBase* app);
        GPUProgramManager(const GPUProgramManager&) = delete;
        GPUProgramManager& operator=(const GPUProgramManager&) = delete;
        GPUProgramManager(GPUProgramManager&&);
        GPUProgramManager& operator=(GPUProgramManager&&);
        virtual ~GPUProgramManager();

        void RecompileAll();

    private:
        void LoadResource(const std::string& resId, ResourceType* resourcePtr) override;
        void HandleShaderCompileException(const shader_compiler_error& except) const;
    };
}

#endif /* GPUPROGRAMMANAGER_H */
