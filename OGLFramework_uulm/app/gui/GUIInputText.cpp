/**
 * @file   GUIInputText.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Februar 2014
 *
 * @brief  Contains the implementation of GUIInputText.
 */

#include "GUIInputText.h"

GUIInputText::GUIInputText(const glm::vec3& relPos, const float length, const guiinfo::input_text_info& iti) :
GUIElementSizeable(relPos),
textRelative(iti.textRel)
{
    text.reset(new ScreenText(*iti.fontInfo->font, iti.fontInfo->fontProgram, "", relPos.xy(),
            iti.fontInfo->fontSize, iti.fontInfo->fontWeight, iti.fontInfo->fontShearing, relPos.z));
    text->SetColor(glm::vec4(iti.fontInfo->color, 1.0f));

    InitializeSize(glm::vec2(length, iti.height), iti.left, iti.right, iti.guiProgram);
}

bool GUIInputText::HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown)
{
    if (isChar) {
        value += static_cast<char> (ch);
    } else if (isbackspace) {
    } else if (isdelete) {
    } else if (isinsert) {
    } else if (isreturn) {
    } else if (istab) {
    } else if (isarrow) {
    } else if (isbeginorend) {
    }
}

bool GUIInputText::HandleMouse(bool, bool, bool, int, const glm::vec2&)
{
}

void GUIInputText::RenderGeometry(GLBatchRenderTarget& rt)
{
    GUIElementSizeable::RenderGeometry(rt);
    if (focus) {
        // TODO: render cursor.
    }
}

void GUIInputText::RenderText(GLBatchRenderTarget & rt)
{
    text->SetPosition(position.xy());
    rt.DrawScreenText(text.get());
}

void GUIInputText::SetText(const std::string & val)
{
    value = val;
    text->SetText(value);
}

std::string GUIInputText::GetText() const
{
    return text;
}
