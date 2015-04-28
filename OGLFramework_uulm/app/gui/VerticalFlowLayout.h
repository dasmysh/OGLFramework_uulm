/**
 * @file   VerticalFlowLayout.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the definition of VerticalFlowLayout.
 */

#ifndef VERTICALFLOWLAYOUT_H
#define VERTICALFLOWLAYOUT_H

#include "GUILayout.h"

/**
 * @brief  Implementation of the GUILayout as vertical flow layout.
 * VerticalFlowLayout objects set the positions of their managed GUIElement objects simply by vertically
 * offsetting the previous elements position and size by a distance between two objects.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 */
class VerticalFlowLayout : public GUILayout
{
public:
    VerticalFlowLayout(float distance = 5.0f);
    VerticalFlowLayout(const glm::vec2& relPos, const glm::vec2& sz, float distance = 5.0f);
    ~VerticalFlowLayout() = default;
    /** Deleted copy constructor. */
    VerticalFlowLayout(const VerticalFlowLayout& orig) = delete;
    /** Deleted copy assignment operator. */
    VerticalFlowLayout& operator=(const VerticalFlowLayout& orig) = delete;
    /** Default move constructor. */
    VerticalFlowLayout(VerticalFlowLayout&& orig) = default;
    /** Default move assignment operator. */
    VerticalFlowLayout& operator=(VerticalFlowLayout&& orig) = default;

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

#endif /* VERTICALFLOWLAYOUT_H */
