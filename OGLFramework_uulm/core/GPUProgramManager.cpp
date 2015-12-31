/**
 * @file   GPUProgramManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.15
 *
 * @brief  Contains the implementation of GPUProgramManager.
 */

#include "GPUProgramManager.h"
#include "gfx/glrenderer/Shader.h"
#include "app/ApplicationBase.h"
#include "app/GLWindow.h"
#include <codecvt>

namespace cgu {

    /**
     * Constructor.
     * @param win the base window this manager belongs to holding all needed dependencies.
     */
    GPUProgramManager::GPUProgramManager(ApplicationBase* app) :
        ResourceManager(app)
    {
    }

    /** Default move constructor. */
    GPUProgramManager::GPUProgramManager(GPUProgramManager&& rhs) : ResourceManagerBase(std::move(rhs)) {}

    /** Default move assignment operator. */
    GPUProgramManager& GPUProgramManager::operator=(GPUProgramManager&& rhs)
    {
        ResourceManagerBase* tResMan = this;
        *tResMan = static_cast<ResourceManagerBase&&>(std::move(rhs));
        return *this;
    }

    GPUProgramManager::~GPUProgramManager() = default;

    void GPUProgramManager::LoadResource(const std::string& resId, ResourceType* resourcePtr)
    {
        try {
            resourcePtr->Load();
        }
        catch (const shader_compiler_error& compilerError) {
            auto filename = boost::get_error_info<boost::errinfo_file_name>(compilerError);
            auto resid = boost::get_error_info<resid_info>(compilerError);
            auto errDesc = boost::get_error_info<errdesc_info>(compilerError);
            auto errorString = boost::get_error_info<compiler_error_info>(compilerError);
            LOG(INFO) << L"Shader compiler/linker error while loading resource \"" << resid->c_str() << L"\"/\"" << resId.c_str() << "\"." << std::endl
                << "Filename: " << (filename == nullptr ? "-" : filename->c_str()) << std::endl
                << "Description: " << (errDesc == nullptr ? "-" : errDesc->c_str()) << std::endl
                << "Compiler Errors: " << (errorString == nullptr ? "-" : errorString->c_str());
            HandleShaderCompileException(compilerError);
        }
        catch (const resource_loading_error& loadingError) {
            auto filename = boost::get_error_info<boost::errinfo_file_name>(loadingError);
            auto resid = boost::get_error_info<resid_info>(loadingError);
            auto fileId = boost::get_error_info<fileid_info>(loadingError);
            auto lineNo = boost::get_error_info<lineno_info>(loadingError);
            auto errDesc = boost::get_error_info<errdesc_info>(loadingError);
            std::stringstream fileline;
            fileline << "FileID/Line Number: ";
            if (fileId == nullptr) fileline << "-/";
            else fileline << *fileId << "/";
            if (lineNo == nullptr) fileline << "-";
            else fileline << *lineNo;
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(INFO) << L"Error while loading program \"" << resid->c_str() << L"\"/\"" << resId.c_str() << "\"." << std::endl
                << "Filename: " << (filename == nullptr ? "-" : filename->c_str()) << std::endl
                << fileline.str().c_str() << std::endl
                << "Description: " << (errDesc == nullptr ? "-" : errDesc->c_str());
            throw;
        }
    }

    /** Recompiles all GPU programs. */
    void GPUProgramManager::RecompileAll()
    {
        for (auto& program : resources) {
            try {
                program.second->RecompileProgram();
            }
            catch (shader_compiler_error compilerError) {
                HandleShaderCompileException(compilerError);
            }
        }
    }

    /**
     * Handles a shader compile exception.
     * @param except the exception to handle
     */
    void GPUProgramManager::HandleShaderCompileException(const shader_compiler_error& except) const
    {
        std::string message = except.what();
        message += "\nRecompile (y) or Quit (n)?";
        if (!application->GetWindow()->MessageBoxQuestion("GPU Program Compiler Error", message)) {
            throw std::runtime_error("GPU program compilation error. Quitting.");
        }
    }
}
