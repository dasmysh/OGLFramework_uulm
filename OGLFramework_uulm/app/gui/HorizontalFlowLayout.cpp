/**
 * @file   HorizontalFlowLayout.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the implementation of HorizontalFlowLayout.
 */

#include "HorizontalFlowLayout.h"
#include <boost/range/adaptor/reversed.hpp>

/**
 * Constructor.
 * @param distance the distance between two elements
 */
HorizontalFlowLayout::HorizontalFlowLayout(float distance) :
HorizontalFlowLayout(glm::vec2(0.0f, 0.0f), glm::vec2(-1.0f, -1.0f), distance)
{
}

/**
 * Constructor.
 * @param relPos the layouts relative position
 * @param sz the layouts size
 * @param distance the distance between two elements
 */
HorizontalFlowLayout::HorizontalFlowLayout(const glm::vec2& relPos, const glm::vec2& sz, float distance) :
GUILayout(relPos, sz),
dist(distance)
{
}

/**
 * Sets the vertical flow layouts alignment
 * (&lt; 0: left/top; == 0: centered; &gt; 0 right/bottom for every direction).
 * @param newAlign the new alignment
 */
void HorizontalFlowLayout::SetAlignment(const glm::i8vec2& newAlign)
{
    align = newAlign;
    RecalculateRelative();
}

/** Recalculates all child elements relative (and absoulte) positions. */
void HorizontalFlowLayout::RecalculateRelative()
{
    float startx = dist;
    if (align.x == 0) {
        float xsize = 0.0f;
        for (auto& element : guiElements) {
            xsize += element->GetSize().x + dist;
        }
        xsize -= dist;
        startx = (size.x - xsize) / 2.0f;
    }

    if (align.x < 0) {
        startx = -startx;
        for (auto& element : boost::adaptors::reverse(guiElements)) {
            float posy = dist;
            if (align.y == 0) {
                posy = (size.y - element->GetSize().y) / 2.0f;
            } else if (align.y < 0) {
                posy = -dist;
            }
            element->SetRelativePosition(glm::vec2(startx, posy));
            startx = startx - element->GetSize().x - dist;
        }
    } else {
        for (auto& element : guiElements) {
            float posy = dist;
            if (align.y == 0) {
                posy = (size.y - element->GetSize().y) / 2.0f;
            } else if (align.y < 0) {
                posy = -dist;
            }
            element->SetRelativePosition(glm::vec2(startx, posy));
            startx = startx + element->GetSize().x + dist;
        }
    }
    GUILayout::RecalculateRelative();
}

/**
 * Adds a new element to the flow layout.
 * @param element the element to add
 */
void HorizontalFlowLayout::AddElement(std::unique_ptr<GUIElement> element)
{
    guiElements.push_back(std::move(element));
    RecalculateRelative();
}
