/**
 * @file   VerticalFlowLayout.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the implementation of VerticalFlowLayout.
 */

#include "VerticalFlowLayout.h"
#include <boost/range/adaptor/reversed.hpp>

/**
 * Constructor.
 * @param distance the distance between two elements
 */
VerticalFlowLayout::VerticalFlowLayout(float distance) :
VerticalFlowLayout(glm::vec2(0.0f, 0.0f), glm::vec2(-1.0f, -1.0f), distance)
{

}

/**
 * Constructor.
 * @param relPos the layouts relative position
 * @param sz the layouts size
 * @param distance the distance between two elements
 */
VerticalFlowLayout::VerticalFlowLayout(const glm::vec2& relPos, const glm::vec2& sz, float distance) :
GUILayout(relPos, sz),
dist(distance)
{

}

/**
 * Sets the vertical flow layouts alignment
 * (&lt; 0: left/top; == 0: centered; &gt; 0 right/bottom for every direction).
 * @param newAlign the new alignment
 */
void VerticalFlowLayout::SetAlignment(const glm::i8vec2& newAlign)
{
    align = newAlign;
    RecalculateRelative();
}

/** Recalculates all child elements relative (and absoulte) positions. */
void VerticalFlowLayout::RecalculateRelative()
{
    float starty = dist;
    if (align.y == 0) {
        float ysize = 0.0f;
        for (auto& element : guiElements) {
            ysize += element->GetSize().y + dist;
        }
        ysize -= dist;
        starty = (size.y - ysize) / 2.0f;
    }

    if (align.y < 0) {
        starty = -starty;
        for (auto& element : boost::adaptors::reverse(guiElements)) {
            float posx = dist;
            if (align.x == 0) {
                posx = (size.x - element->GetSize().x) / 2.0f;
            } else if (align.x < 0) {
                posx = -dist;
            }
            element->SetRelativePosition(glm::vec2(posx, starty));
            starty = starty - element->GetSize().y - dist;
        }
    } else {
        for (auto& element : guiElements) {
            float posx = dist;
            if (align.x == 0) {
                posx = (size.x - element->GetSize().x) / 2.0f;
            } else if (align.x < 0) {
                posx = -dist;
            }
            element->SetRelativePosition(glm::vec2(posx, starty));
            starty = starty + element->GetSize().y + dist;
        }
    }
    GUILayout::RecalculateRelative();
}

/**
 * Adds a new element to the flow layout.
 * @param element the element to add
 */
void VerticalFlowLayout::AddElement(std::unique_ptr<GUIElement> element)
{
    guiElements.push_back(std::move(element));
    RecalculateRelative();
}
