/**
 * @file   ShaderManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the implementation of ShaderManager.
 */

#include "ShaderManager.h"

/**
 * Constructor.
 * @param win the base window this manager belongs to holding all needed dependencies.
 */
ShaderManager::ShaderManager(ApplicationBase* app) :
shaders(),
application(app)
{
}

ShaderManager::ShaderType* ShaderManager::GetResource(const std::string& resId)
{
    try {
        return shaders.at(resId).get();
    } catch (std::out_of_range e) {
        LOG(INFO) << L"No resource with id \"" << resId << L"\" found. Creating new one.";
        std::unique_ptr<ShaderType> shaderPtr(new ShaderType(resId, application));
        ShaderType* shaderRawPtr = shaderPtr.get();
        // no exception handling, this is done by the program manager
        shaderPtr->Load();
        shaders.insert(std::make_pair(resId, std::move(shaderPtr)));
        return shaderRawPtr;
    }
}

bool ShaderManager::HasResource(const std::string& resId)
{
    auto got = shaders.find(resId);
    if (got == shaders.end())
        return false;
    return true;
}
