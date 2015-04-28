/**
 * @file   DialogManager.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Februar 2014
 *
 * @brief  Contains the implementation of DialogManager.
 */

#include <vector>
#include <memory>

#include "DialogManager.h"
#include "GUILabel.h"
#include "GUITheme.h"

/**
 * Constructor.
 * @param aspectRatio the dialogs aspect ratio
 */
DialogManager::DialogManager(float aspectRatio, GUITheme* theme, float layer) :
ar(aspectRatio),
guitheme(theme),
depth(layer)
{
}

/**
 * Calculates the child elements size.
 * @param element the child element to calculate the size for
 */
void DialogManager::CalcSubElementSize(GUIElement* element)
{
    glm::vec2 virtualPos(0.0f, 0.0f);
    glm::vec2 virtualSize(ar * 1000.0f, 1000.0f);
    element->CalculateAbsolute(virtualPos, virtualSize);
}

/**
 * Adds a new element to the dialog.
 * @param element the element to add
 */
void DialogManager::AddElement(std::unique_ptr<GUIElement> element)
{
    CalcSubElementSize(element.get());
    guiElements.push_back(std::move(element));
}

/**
 * Resizes the dialog. As the height of the manager is fixed (1000.0f) only the aspect ratio is
 * needed to calculate the width.
 * @param aspectRatio the new aspect ratio.
 */
void DialogManager::Resize(float aspectRatio)
{
    ar = aspectRatio;
    for (auto& element : guiElements) {
        CalcSubElementSize(element.get());
    }
}

/**
 * Renders the elements geometry.
 * @param rt the render target
 */
void DialogManager::RenderGeometry(GLBatchRenderTarget& rt)
{
    for (auto& element : guiElements) {
        element->RenderGeometry(rt);
    }
}

/**
 * Renders the elements text.
 * @param rt the render target
 */

void DialogManager::RenderText(GLBatchRenderTarget& rt)
{
    for (auto& element : guiElements) {
        element->RenderText(rt);
    }
}

/**
 * Handles keyboard actions.
 * @param ch the key related to the action (virtual key code: PK_*)
 * @param bKeyDown was the key pressed
 * @param bAltDown was the "Alt"-key pressed at the same time
 * @return <code>true</code> if some element handled the input
 */
bool DialogManager::HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown)
{
    return focusedElement->HandleKeyboardComplete(ch, bKeyDown, bAltDown);
}

/**
 * Handles mouse actions.
 * @param bLeftButtonDown is the left mouse button pressed
 * @param bRightButtonDown is the right mouse button pressed
 * @param bMiddleButtonDown is the middle mouse button pressed
 * @param nMouseWheelDelta the mouse wheel delte
 * @param pos the current mouse position
 * @return <code>true</code> if some element handled the input
 */
bool DialogManager::HandleMouse(bool bLeftButtonDown, bool bRightButtonDown,
        bool bMiddleButtonDown, int nMouseWheelDelta, const glm::vec2& pos)
{
    if (!focusedElement->IsContained(pos)) {
        focusedElement->SetFocus(false);
        focusedElement = nullptr;
        for (auto& element : guiElements) {
            if (element->IsContained(pos)) {
                focusedElement = element.get();
                focusedElement->SetFocus(true);
                break;
            }
        }
    }

    if (focusedElement) {
        return focusedElement->HandleMouse(bLeftButtonDown, bRightButtonDown,
                bMiddleButtonDown, nMouseWheelDelta, pos);
    }
    return false;
}

/**
 * Creates a new GUILabel for this dialog.
 * @param pos the position of the label
 * @param text the labels text
 * @return the created label
 */
std::unique_ptr<GUILabel> DialogManager::CreateLabel(const glm::vec2& pos, const std::string& text)
{
    return std::unique_ptr<GUILabel>(new GUILabel(glm::vec3(pos, depth),
            guitheme->GetFontInfo(), text));
}
