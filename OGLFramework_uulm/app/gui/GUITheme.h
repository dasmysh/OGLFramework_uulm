/**
 * @file   GUITheme.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the definition of GUITheme.
 */

#ifndef GUITHEME_H
#define GUITHEME_H

#include "gui_info.h"
#include "gfx/glrenderer/SimpleQuadRenderable.h"
#include "gfx/glrenderer/GUIRenderable.h"

class ApplicationBase;
class GLTexture2D;

/**
 * @brief
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 */
class GUITheme : public Resource
{
public:
    GUITheme(const std::string& fontName, ApplicationBase* app);
    virtual ~GUITheme();
    /** Deleted copy constructor. */
    GUITheme(const GUITheme& orig) = delete;
    /** Deleted copy assignment operator. */
    GUITheme& operator=(const GUITheme& orig) = delete;
    /** Default move constructor. */
    GUITheme(GUITheme&& orig) noexcept;
    /** Default move assignment operator. */
    GUITheme& operator=(GUITheme&& orig) noexcept;

    virtual void Load();
    virtual void Unload();

    const guiinfo::font_info& GetFontInfo() const;
    const guiinfo::button_info& GetButtonInfo() const;
    const guiinfo::input_text_info& GetInputTextInfo() const;
    const guiinfo::slider_info& GetSliderInfo() const;
    const guiinfo::checkbox_info& GetCheckboxInfo() const;
    const guiinfo::radio_button_info& GetRadioButtonInfo() const;

private:
    void UnloadLocal();

    std::array<std::unique_ptr<GUIRenderable>, 2> sliderMarker = {
        {nullptr, nullptr}
    };
    std::array<std::unique_ptr<GUIRenderable>, 2> checkboxChecker = {
        {nullptr, nullptr}};
    std::array<std::unique_ptr<GUIRenderable>, 2> checkboxChecked = {
        {nullptr, nullptr}};
    std::array<std::unique_ptr<GUIRenderable>, 2> radioButtonChecker = {
        {nullptr, nullptr}};
    std::array<std::unique_ptr<GUIRenderable>, 2> radioButtonChecked = {
        {nullptr, nullptr}};
    std::unique_ptr<SimpleQuadRenderable> cursor = {};
    std::unique_ptr<guiinfo::font_info> font = {};
    std::unique_ptr<guiinfo::button_info> button = {};
    std::unique_ptr<guiinfo::input_text_info> inputText = {};
    std::unique_ptr<guiinfo::slider_info> slider = {};
    std::unique_ptr<guiinfo::checkbox_info> checkbox = {};
    std::unique_ptr<guiinfo::radio_button_info> radioButton = {};

    GLTexture2D* themeTexture = nullptr;
};

#endif /* GUITHEME_H */
