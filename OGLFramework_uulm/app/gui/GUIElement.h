/**
 * @file   GUIElement.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Februar 2014
 *
 * @brief  Contains the definition of GUIElement.
 */

#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include "main.h"

class GLBatchRenderTarget;

/**
 * @brief  Base class for all gui elements.
 * Positioning works like this: positive values are top/left, negative are bottom/right aligned.
 * These are relative positions. The real positions are calculated using these values. Negative
 * Size values mean "size of parent" (minus offsets). All coordinates are in virtual screen
 * coordinates. The height of the window/screen is always 1000.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Februar 2014
 */
class GUIElement
{
public:
    GUIElement(const glm::vec3& relPos, const glm::vec2& sz);
    virtual ~GUIElement() = default;
    /** Deleted copy constructor. */
    GUIElement(const GUIElement& orig) = delete;
    /** Deleted copy assignment operator. */
    GUIElement& operator=(const GUIElement& orig) = delete;
    /** Default move constructor. */
    GUIElement(GUIElement&& orig) = delete;
    /** Default move assignment operator. */
    GUIElement& operator=(GUIElement&& orig) = delete;

    /**
     * Handles keyboard actions.
     * @param ch the key related to the action (virtual key code: PK_*)
     * @param bKeyDown was the key pressed
     * @param bAltDown was the "Alt"-key pressed at the same time
     * @return <code>true</code> if some element handled the input
     */
    virtual bool HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown) = 0;
    /**
     * Handles mouse actions.
     * @param bLeftButtonDown is the left mouse button pressed
     * @param bRightButtonDown is the right mouse button pressed
     * @param bMiddleButtonDown is the middle mouse button pressed
     * @param nMouseWheelDelta the mouse wheel delta
     * @param pos the current mouse position
     * @return <code>true</code> if some element handled the input
     */
    virtual bool HandleMouse(bool bLeftButtonDown, bool bRightButtonDown,
            bool bMiddleButtonDown, int nMouseWheelDelta, const glm::vec2& pos) = 0;

    bool IsContained(const glm::vec2& pos);
    virtual void CalculateAbsolute(const glm::vec2& contPos, const glm::vec2& contSize);
    void SetRelativePosition(const glm::vec2& relPos);
    void SetSize(const glm::vec2& sz);

    /**
     * Renders the elements geometry.
     * @param rt the render target
     */
    virtual void RenderGeometry(GLBatchRenderTarget& rt) = 0;
    /**
     * Renders the elements text.
     * @param rt the render target
     */
    virtual void RenderText(GLBatchRenderTarget& rt) = 0;
    /**
     * Updates the current frame.
     * @param time the time elapsed since the application started
     * @param elapsed the time elapsed since the last frame
     */
    virtual void UpdateFrame(float time, float elapsed) = 0;

    const glm::vec2& GetRelativePosition() const;
    const glm::vec2& GetAdjustedSize() const;
    const glm::vec2& GetSize() const;
    bool IsFocus() const;
    void SetFocus(bool f);

protected:
    /** Holds the relative position of the element. */
    glm::vec2 relativePos;
    /** Holds the absolute (top left) position of the element. */
    glm::vec3 position;
    /** Holds the size of the element. */
    glm::vec2 size;
    /** Holds the size adjusted to the size of the parent element. */
    glm::vec2 adjustedSize;
    /** Holds whether the element is focused. */
    bool focus;

};

#endif /* GUIELEMENT_H */
