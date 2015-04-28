/**
 * @file   GUILayout.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the definition of GUILayout.
 */

#ifndef GUILAYOUT_H
#define GUILAYOUT_H

#include "GUIElement.h"

/**
 * @brief  Base class for all GUI layouts.
 *
 * Some layouts use alignment. By default all elements are align top left. This is represented by an alignment of
 * (1|1). Other alignments are represented by negative numbers (bottom/right) or zeros (centered).
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 */
class GUILayout : public GUIElement
{
public:
    GUILayout(const glm::vec2& relPos, const glm::vec2& sz);
    virtual ~GUILayout() = default;
    /** Deleted copy constructor. */
    GUILayout(const GUILayout& orig) = delete;
    /** Deleted copy assignment operator. */
    GUILayout& operator=(const GUILayout& orig) = delete;
    /** Default move constructor. */
    GUILayout(GUILayout&& orig) = default;
    /** Default move assignment operator. */
    GUILayout& operator=(GUILayout&& orig) = default;

    virtual bool HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown) override;
    virtual bool HandleMouse(bool bLeftButtonDown, bool bRightButtonDown,
            bool bMiddleButtonDown, int nMouseWheelDelta, const glm::vec2& pos) override;
    void CalculateAbsolute(const glm::vec2& contPos, const glm::vec2& contSize) override;

    virtual void Render() override;

protected:
    void AddElement(std::unique_ptr<GUIElement> element);
    void FocusElement(GUIElement* element);
    const GUIElement* GetLastElement() const;
    virtual void RecalculateRelative();

    /** The type of the list of child elements. */
    using ElementList = std::vector<std::unique_ptr<GUIElement> >;
    /** Holds all registered gui elements. */
    ElementList guiElements = {};

private:
    /** Holds the gui element currently in focus. */
    GUIElement* focusedElement = nullptr;

};

#endif /* GUILAYOUT_H */
