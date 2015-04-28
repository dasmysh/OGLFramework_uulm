/**
 * @file   GUIButton.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the implementation of GUIButton.
 */

#define GLM_SWIZZLE

#include "GUIButton.h"
#include "gfx/glrenderer/GUIRenderable.h"

GUIButton::GUIButton(const glm::vec3& relPos, const std::string& txt, std::function<void() > onClick,
        const guiinfo::button_info& bi) :
GUIElementSizeable(relPos),
textRelative(bi.textRel),
onClickHandler(onClick)
{
    text.reset(new ScreenText(*bi.fontInfo->font, bi.fontInfo->fontProgram, txt, relPos.xy(),
            bi.fontInfo->fontSize, bi.fontInfo->fontWeight, bi.fontInfo->fontShearing, relPos.z));
    text->SetColor(glm::vec4(bi.fontInfo->color, 1.0f));

    InitializeSize(glm::vec2(text->GetPixelLength() + 2.0f * textRelative.x, bi.height),
            bi.left, bi.right, bi.guiProgram);
}

bool GUIButton::HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool)
{
    if (ch == PK_RETURN && bKeyDown) {
        onClickHandler();
        return true;
    }
    return false;
}

bool GUIButton::HandleMouse(bool bLeftButtonDown, bool, bool, int, const glm::vec2 & pos)
{
    if (bLeftButtonDown && IsContained(pos)) {
        onClickHandler();
        return true;
    }
    return false;
}

void GUIButton::RenderText(GLBatchRenderTarget & rt)
{
    text->SetPosition(position.xy() + textRelative);
    rt.DrawScreenText(text.get());
}
