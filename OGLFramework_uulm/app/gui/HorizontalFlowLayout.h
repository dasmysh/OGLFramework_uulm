/**
 * @file   HorizontalFlowLayout.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the definition of HorizontalFlowLayout.
 */

#ifndef HORIZONTALFLOWLAYOUT_H
#define HORIZONTALFLOWLAYOUT_H

#include "app/gui/GUILayout.h"

/**
 * @brief  Implementation of the GUILayout as flow layout.
 * HorizontalFlowLayout objects set the positions of their managed GUIElement objects simply by horizontally
 * offsetting the previous elements position and size by a distance between two objects.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 */
class HorizontalFlowLayout : public GUILayout
{
public:
    HorizontalFlowLayout(float distance = 5.0f);
    HorizontalFlowLayout(const glm::vec2& relPos, const glm::vec2& sz, float distance = 5.0f);
    ~HorizontalFlowLayout() = default;
    /** Deleted copy constructor. */
    HorizontalFlowLayout(const HorizontalFlowLayout& orig) = delete;
    /** Deleted copy assignment operator. */
    HorizontalFlowLayout& operator=(const HorizontalFlowLayout& orig) = delete;
    /** Default move constructor. */
    HorizontalFlowLayout(HorizontalFlowLayout&& orig) = default;
    /** Default move assignment operator. */
    HorizontalFlowLayout& operator=(HorizontalFlowLayout&& orig) = default;

    void AddElement(std::unique_ptr<GUIElement> element);
    void SetAlignment(const glm::i8vec2& newAlign);

protected:
    virtual void RecalculateRelative() override;

private:
    /** Holds the flow distance. */
    float dist;
    /** Holds the alignment. */
    glm::i8vec2 align = glm::i8vec2(1, 1);

};

#endif /* HORIZONTALFLOWLAYOUT_H */
