/**
 * @file   GUILabel.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the implementation of GUILabel.
 */

#define GLM_SWIZZLE
#include "GUILabel.h"
#include "gfx/glrenderer/ScreenText.h"

GUILabel::GUILabel(const glm::vec3& relPos, const guiinfo::font_info& fi, const std::string& txt) :
GUIElement(relPos, glm::vec2(0.0f, 0.0f))
{
    text.reset(new ScreenText(*fi.font, fi.fontProgram, txt, relPos.xy(), fi.fontSize,
            fi.fontWeight, fi.fontShearing, relPos.z));
    text->SetColor(glm::vec4(fi.color, 1.0f));
    SetSize(glm::vec2(text->GetPixelLength(), fi.fontSize));
}

bool GUILabel::HandleKeyboardComplete(unsigned int, bool, bool)
{
    return false;
}

bool GUILabel::HandleMouse(bool, bool, bool, int, const glm::vec2&)
{
    return false;
}

void GUILabel::RenderGeometry(GLBatchRenderTarget&)
{
}

void GUILabel::RenderText(GLBatchRenderTarget& rt)
{
    text->SetPosition(position.xy());
    rt.DrawScreenText(text.get());
}
