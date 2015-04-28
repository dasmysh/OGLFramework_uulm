/**
 * @file   GUIThemeManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the implementation of GUIThemeManager.
 */

#include "GUIThemeManager.h"
#include "app/ApplicationBase.h"

GUIThemeManager::GUIThemeManager(ApplicationBase* app) :
application(app)
{
}

typename GUIThemeManager::GUIThemePtrUtil::SharedTPtr GUIThemeManager::GetResource(const std::string& resId)
{
    try {
        return GUIThemePtrUtil::share(themes.at(resId));
    } catch (std::out_of_range e) {
        LOG(Log::LogLevel::LL_INFO) << L"No resource with id \"" << std::wstring(resId.begin(), resId.end())
                << L"\" found. Creating new one.";
        typename GUIThemePtrUtil::TPtr themePtr = GUIThemePtrUtil::template make(resId, application);
        typename GUIThemePtrUtil::SharedTPtr themeRawPtr = GUIThemePtrUtil::share(themePtr);
        themes.insert(std::make_pair(resId, std::move(themePtr)));
        return themeRawPtr;
    }
}

bool GUIThemeManager::HasResource(const std::string& resId)
{
    auto got = themes.find(resId);
    if (got == themes.end())
        return false;
    return true;
}
