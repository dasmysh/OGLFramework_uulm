/**
 * @file   GUIInputText.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the definition of GUIInputText.
 */

#ifndef GUIINPUTTEXT_H
#define GUIINPUTTEXT_H

#include "GUIElementSizeable.h"
#include "gui_info.h"

/**
 * @brief
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 */
class GUIInputText : public GUIElementSizeable
{
public:
    GUIInputText(const glm::vec3& relPos, const float length, const guiinfo::input_text_info& iti);
    virtual ~GUIInputText() = default;
    /** Deleted copy constructor. */
    GUIInputText(const GUIInputText& orig) = delete;
    /** Deleted copy assignment operator. */
    GUIInputText& operator=(const GUIInputText& orig) = delete;
    /** Default move constructor. */
    GUIInputText(GUIInputText&& orig) = default;
    /** Default move assignment operator. */
    GUIInputText& operator=(GUIInputText&& orig) = default;

    virtual bool HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown) override;
    virtual bool HandleMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
            int nMouseWheelDelta, const glm::vec2& pos) override;
    virtual void RenderText(GLBatchRenderTarget& rt) override;
    virtual void RenderGeometry(GLBatchRenderTarget& rt) override;
    void SetText(const std::string& val);
    std::string GetText() const;

private:
    /** Holds the string value. */
    std::string value = "";
    /** Holds the text. */
    std::unique_ptr<ScreenText> text = {};
    /** Holds the texts relative position. */
    glm::vec2 textRelative = {};

};

#endif /* GUIINPUTTEXT_H */
