/**
 * @file   GUILabel.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the definition of GUILabel.
 */

#ifndef GUILABEL_H
#define GUILABEL_H

#include "GUIElement.h"
#include "gfx/glrenderer/ScreenText.h"
#include "gui_info.h"

/**
 * @brief
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 */
class GUILabel : public GUIElement
{
public:
    GUILabel(const glm::vec3& relPos, const guiinfo::font_info& fi, const std::string& txt);
    virtual ~GUILabel() = default;
    /** Deleted copy constructor. */
    GUILabel(const GUILabel& orig) = delete;
    /** Deleted copy assignment operator. */
    GUILabel& operator=(const GUILabel& orig) = delete;
    /** Default move constructor. */
    GUILabel(GUILabel&& orig) = default;
    /** Default move assignment operator. */
    GUILabel& operator=(GUILabel&& orig) = default;

    virtual bool HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown) override;
    virtual bool HandleMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
            int nMouseWheelDelta, const glm::vec2& pos) override;
    virtual void RenderGeometry(GLBatchRenderTarget& rt) override;
    virtual void RenderText(GLBatchRenderTarget& rt) override;

private:
    /** Holds the rendered text. */
    std::unique_ptr<ScreenText> text = {};

};

#endif /* GUILABEL_H */
