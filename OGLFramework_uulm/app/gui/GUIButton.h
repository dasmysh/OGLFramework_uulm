/**
 * @file   GUIButton.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the definition of GUIButton.
 */

#ifndef GUIBUTTON_H
#define GUIBUTTON_H

#include "GUIElementSizeable.h"
#include "gfx/glrenderer/ScreenText.h"
#include "gui_info.h"

/**
 * @brief  Button implementation for the gui.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 */
class GUIButton : public GUIElementSizeable
{
public:
    GUIButton(const glm::vec3& relPos, const std::string& txt, std::function<void() > onClick,
            const guiinfo::button_info& bi);
    virtual ~GUIButton() = default;
    /** Deleted copy constructor. */
    GUIButton(const GUIButton& orig) = delete;
    /** Deleted copy assignment operator. */
    GUIButton& operator=(const GUIButton& orig) = delete;
    /** Default move constructor. */
    GUIButton(GUIButton&& orig) = default;
    /** Default move assignment operator. */
    GUIButton& operator=(GUIButton&& orig) = default;

    virtual bool HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown) override;
    virtual bool HandleMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
            int nMouseWheelDelta, const glm::vec2& pos) override;
    virtual void RenderText(GLBatchRenderTarget& rt) override;

private:
    /** Holds the buttons text. */
    std::unique_ptr<ScreenText> text = {};
    /** Holds the texts relative position. */
    glm::vec2 textRelative = {};
    /** Holds the on click event handler. */
    std::function<void() > onClickHandler;

};

#endif /* GUIBUTTON_H */
