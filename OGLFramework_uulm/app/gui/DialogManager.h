/**
 * @file   DialogManager.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Februar 2014
 *
 * @brief  Contains the definition of DialogManager.
 */

#ifndef DIALOGMANAGER_H
#define DIALOGMANAGER_H

#include "GUIElement.h"
#include "GUILabel.h"
#include "gfx/glrenderer/Font.h"

class GUITheme;

/**
 * @brief  Manager for all dialog elements shown on a single render target.
 * Possible overlay dialog elements will use their own dialog manager
 * (but could still be managed as sub-elements ...)
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   17. Februar 2014
 */
class DialogManager
{
public:
    DialogManager(float aspectRatio, GUITheme* theme, float layer);
    virtual ~DialogManager() = default;
    /** Deleted copy constructor. */
    DialogManager(const DialogManager& orig) = delete;
    /** Deleted copy assignment operator. */
    DialogManager& operator=(const DialogManager& orig) = delete;
    /** Default move constructor. */
    DialogManager(DialogManager&& orig) = default;
    /** Default move assignment operator. */
    DialogManager& operator=(DialogManager&& orig) = default;

    void AddElement(std::unique_ptr<GUIElement> element);

    bool HandleKeyboardComplete(unsigned int ch, bool bKeyDown, bool bAltDown);
    bool HandleMouse(bool bLeftButtonDown, bool bRightButtonDown,
            bool bMiddleButtonDown, int nMouseWheelDelta, const glm::vec2& pos);
    void Resize(float aspectRatio);

    void RenderGeometry(GLBatchRenderTarget& rt);
    void RenderText(GLBatchRenderTarget& rt);
    void UpdateFrame(float time, float elapsed);

    std::unique_ptr<GUILabel> CreateLabel(const glm::vec2& pos, const std::string& text);

private:
    /** Holds the current aspect ratio used. */
    float ar = 1.0f;
    /** Holds all registered gui elements. */
    std::vector<std::unique_ptr<GUIElement> > guiElements = {};
    /** Holds the gui element currently in focus. */
    GUIElement* focusedElement = nullptr;
    /** Holds the theme used by the gui. */
    GUITheme* guitheme;
    /** Holds the depth layer of this dialog. */
    float depth;

    void CalcSubElementSize(GUIElement* element);
    // TODO: Layouts: grid, flow, groups

};

#endif /* DIALOGMANAGER_H */
