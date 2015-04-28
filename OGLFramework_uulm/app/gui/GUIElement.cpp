/**
 * @file   GUIElement.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Februar 2014
 *
 * @brief  Contains the implementation of GUIElement.
 */

#include <cmath>

#include "GUIElement.h"

/**
 * Constructor.
 * @param relPos the elements position relative to its parent
 * @param sz the elements size
 */
GUIElement::GUIElement(const glm::vec2& relPos, const glm::vec2& sz) :
relativePos(relPos),
position(relPos),
size(sz),
adjustedSize(sz),
focus(false)
{
}

/**
 * Is the given position contained in the element
 * @param pos the position to check
 * @return <code>true</code> if the position was contained
 */
bool GUIElement::IsContained(const glm::vec2& pos)
{
    glm::vec2 botRight = position + size;
    return pos.x >= position.x && pos.x <= botRight.x && pos.y >= position.y && pos.y <= botRight.y;
}

/**
 * Calculate the elements absolute position (and size) based on the absolute position and and size of its parent.
 * @param contPos the parents absolute position
 * @param contSize the parents size
 */
void GUIElement::CalculateAbsolute(const glm::vec2& contPos, const glm::vec2& contSize)
{
    glm::vec2 contBottomRight = contPos + contSize;
    adjustedSize.x = size.x > 0.0f ? size.x : contSize.x;
    adjustedSize.y = size.y > 0.0f ? size.y : contSize.y;
    if (std::signbit(relativePos.x)) {
        position.x = contBottomRight.x - size.x + relativePos.x;
        if (position.x < contPos.x) {
            adjustedSize.x = size.x - (contPos.x - position.x);
            position.x = contPos.x;
        }
    } else {
        position.x = contPos.x + relativePos.x;
        if (relativePos.x + size.x > contSize.x) {
            adjustedSize.x = contSize.x - relativePos.x;
        }
    }

    if (std::signbit(relativePos.y)) {
        position.y = contBottomRight.y - size.y + relativePos.y;
        if (position.y < contPos.y) {
            adjustedSize.y = size.y - (contPos.y - position.y);
            position.y = contPos.y;
        }
    } else {
        position.y = contPos.y + relativePos.y;
        if (relativePos.y + size.y > contSize.y) {
            adjustedSize.y = contSize.y - relativePos.y;
        }
    }
}

/**
 * Sets the elements relative position.
 * @param relPos the new relative position
 */
void GUIElement::SetRelativePosition(const glm::vec2& relPos)
{
    relativePos = relPos;
}

/**
 * Gets the elements adjusted size
 * @return the adjusted size
 */
const glm::vec2& GUIElement::GetAdjustedSize() const
{
    return adjustedSize;
}

/**
 * Gets the elements size.
 * @return the size
 */
const glm::vec2& GUIElement::GetSize() const
{
    return size;
}

void GUIElement::SetSize(const glm::vec2& sz)
{
    size = sz;
}

/**
 * Returns the elements relative position
 * @return the elements relative position
 */
const glm::vec2& GUIElement::GetRelativePosition() const
{
    return relativePos;
}

bool GUIElement::IsFocus() const
{
    return focus;
}

void GUIElement::SetFocus(bool f)
{
    focus = f;
}
