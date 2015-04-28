/**
 * @file   GUILayout.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the implementation of GUILayout.
 */

#include "GUILayout.h"

/**
 * Constructor.
 * @param relPos the position relative to the layouts parent element
 * @param sz the layouts size
 */
GUILayout::GUILayout(const glm::vec2& relPos, const glm::vec2& sz) :
GUIElement(relPos, sz)
{
}

/**
 * Sets the focus of this layout.
 * @param element the element to focus.
 */
void GUILayout::FocusElement(GUIElement* element)
{
    if (focusedElement) focusedElement->SetFocus(false);
    focusedElement = element;
    focusedElement->SetFocus(true);
}

/**
 * Adds a new GUIElement to the layout. Calculates its absolute position based on the layouts position and size.
 * @param element the element to add.
 */
void GUILayout::AddElement(std::unique_ptr<GUIElement> element)
{
    element->CalculateAbsolute(position, adjustedSize);
    guiElements.push_back(std::move(element));
}

/**
 * Returns the last GUIElement that has been added to the layout.
 * @return the last GUIElement
 */
const GUIElement* GUILayout::GetLastElement() const
{
    if (guiElements.size() == 0) {
        return nullptr;
    }
    return guiElements.back().get();
}

bool GUILayout::HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown)
{
    return focusedElement->HandleKeyboardComplete(ch, bKeyDown, bAltDown);
}

bool GUILayout::HandleMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
        int nMouseWheelDelta, const glm::vec2& pos)
{
    if (!focusedElement->IsContained(pos)) {
        focusedElement->SetFocus(false);
        focusedElement = nullptr;
        for (auto& element : guiElements) {
            if (element->IsContained(pos)) {
                FocusElement(element.get());
                break;
            }
        }
    }

    if (focusedElement) {
        return focusedElement->HandleMouse(bLeftButtonDown, bRightButtonDown, bMiddleButtonDown,
                nMouseWheelDelta, pos);
    }

    return false;
}

void GUILayout::CalculateAbsolute(const glm::vec2& contPos, const glm::vec2& contSize)
{
    GUIElement::CalculateAbsolute(contPos, contSize);
    RecalculateRelative();
}

/**
 * Recalculates the relative positions of the child elements (and also updates the absolute positions).
 */
void GUILayout::RecalculateRelative()
{
    for (auto& element : guiElements) {
        element->CalculateAbsolute(position, adjustedSize);
    }
}

void GUILayout::Render()
{
    for (auto& element : guiElements) {
        element->Render();
    }
}