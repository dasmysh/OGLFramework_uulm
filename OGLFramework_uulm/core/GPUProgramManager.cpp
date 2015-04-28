/**
 * @file   GPUProgramManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the implementation of GPUProgramManager.
 */

#include "GPUProgramManager.h"
#include "app/ApplicationBase.h"

/**
 * Constructor.
 * @param win the base window this manager belongs to holding all needed dependencies.
 */
GPUProgramManager::GPUProgramManager(ApplicationBase* app) :
gpuPrograms(),
application(app)
{
}

GPUProgramManager::GPUProgramType* GPUProgramManager::GetResource(const std::string& resId)
{
    try {
        return gpuPrograms.at(resId).get();
    } catch (std::out_of_range e) {
        LOG(INFO) << L"No resource with id \"" << resId << L"\" found. Creating new one.";
        std::unique_ptr<GPUProgramType> gpuProgPtr(new GPUProgramType(resId, application));
        GPUProgramType* gpuProgRawPtr = gpuProgPtr.get();
        while (!gpuProgPtr->IsLoaded()) {
            try {
                gpuProgPtr->Load();
            } catch (shader_compiler_error compilerError) {
                HandleShaderCompileException(compilerError);
            }
        }
        gpuPrograms.insert(std::make_pair(resId, std::move(gpuProgPtr)));
        return gpuProgRawPtr;
    }
}

bool GPUProgramManager::HasResource(const std::string& resId)
{
    auto got = gpuPrograms.find(resId);
    if (got == gpuPrograms.end())
        return false;
    return true;
}

/** Recompiles all GPU programs. */
void GPUProgramManager::RecompileAll()
{
    for (auto& program : gpuPrograms) {
        try {
            program.second->RecompileProgram();
        } catch (shader_compiler_error compilerError) {
            HandleShaderCompileException(compilerError);
        }
    }
}

/**
 * Handles a shader compile exception.
 * @param except the exception to handle
 */
void GPUProgramManager::HandleShaderCompileException(const shader_compiler_error& except)
{
    std::string message = except.what();
    message += "\nRecompile (y) or Quit (n)?";
    if (!application->GetWindow()->MessageBoxQuestion("GPU Program Compiler Error", message)) {
        throw std::runtime_error("GPU program compilation error. Quitting.");
    }
}
