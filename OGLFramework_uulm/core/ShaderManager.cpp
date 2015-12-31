/**
 * @file   ShaderManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.15
 *
 * @brief  Contains the implementation of ShaderManager.
 */

#include "ShaderManager.h"

namespace cgu {
    /**
     * Constructor.
     * @param win the base window this manager belongs to holding all needed dependencies.
     */
    ShaderManager::ShaderManager(ApplicationBase* app) :
        ResourceManager(app)
    {
    }

    /** Default move constructor. */
    ShaderManager::ShaderManager(ShaderManager&& rhs) : ResourceManagerBase(std::move(rhs)) {}

    /** Default move assignment operator. */
    ShaderManager& ShaderManager::operator=(ShaderManager&& rhs)
    {
        ResourceManagerBase* tResMan = this;
        *tResMan = static_cast<ResourceManagerBase&&>(std::move(rhs));
        return *this;
    }

    /** Default destructor. */
    ShaderManager::~ShaderManager() = default;

    void ShaderManager::LoadResource(const std::string& resId, ResourceType* resourcePtr)
    {
        try {
            resourcePtr->Load();
        }
        catch (const shader_compiler_error&) {
            // no compiler error handling, this is done by the program manager
            throw;
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
            LOG(INFO) << L"Error while loading shader \"" << resid->c_str() << L"\"/\"" << resId.c_str() << "\"." << std::endl
                << "Filename: " << (filename == nullptr ? "-" : *filename) << std::endl
                << fileline.str().c_str() << std::endl
                << "Description: " << (errDesc == nullptr ? "-" : *errDesc);
            throw;
        }
    }
}
