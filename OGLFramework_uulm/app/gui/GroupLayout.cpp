/**
 * @file   GroupLayout.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the implementation of GroupLayout.
 */

#include <bits/unique_ptr.h>

#include "GroupLayout.h"

namespace grplayout
{

    /**
     * Adds a new element (id) to the parallel group.
     * @param elementId the elements id
     * @return the parallel group for further adding of elements
     */
    parallel_group* parallel_group::AddElement(unsigned int elementId)
    {
        elementIds.push_back(elementId);
        return this;
    }

    /**
     * Returns the biggest height of the elements contained.
     * @param elements the elements the ids refer to
     * @return the maximum height in the group
     */
    float parallel_group::GetMaxHeight(const std::vector<std::unique_ptr<GUIElement> >& elements)
    {
        float result = 0.0f;
        for (unsigned int id : elementIds) {
            result = std::max(elements[id]->GetSize().y, result);
        }
        return result;
    }

    /**
     * Returns the biggest width of the elements contained.
     * @param elements the elements the ids refer to
     * @return the maximum width in the group
     */
    float parallel_group::GetMaxWidth(const std::vector<std::unique_ptr<GUIElement> >& elements)
    {
        float result = 0.0f;
        for (unsigned int id : elementIds) {
            result = std::max(elements[id]->GetSize().x, result);
        }
        return result;
    }

    /**
     * Adds a new single element to the group. It will be wrapped as an
     * parallel_group with only one object.
     * @param elementId the elements id to add
     * @return the group for further adding of elements
     */
    sequential_group* sequential_group::AddElement(unsigned int elementId)
    {
        group_element ge(new parallel_group);
        ge->AddElement(elementId);
        elements.push_back(std::move(ge));
        return this;
    }

    /**
     * Adds a parallel_group to this one.
     * @param group the group to add
     * @return the group for further adding of elements
     */
    sequential_group* sequential_group::AddGroup(std::unique_ptr<parallel_group> group)
    {
        elements.push_back(std::move(group));
        return this;
    }
}

/**
 * Constructor.
 * @param distance the (half) distance between two elements
 */
GroupLayout::GroupLayout(float distance) :
GridLayout(0, 0, distance)
{
}

/**
 * Creates a new sequential_group.
 * @return the group
 */
std::unique_ptr<grplayout::sequential_group> GroupLayout::CreateSequentialGroup()
{
    using namespace grplayout;
    return std::unique_ptr<sequential_group>(new sequential_group);
}

/**
 * Creates a new parallel_group with a certain alignment.
 * @param align the alignment of the new group
 * @return the group
 */
std::unique_ptr<grplayout::parallel_group> GroupLayout::CreateParallelGroup(glm::i8 align)
{
    using namespace grplayout;
    std::unique_ptr<parallel_group> pgrp(new parallel_group);
    pgrp->align = align;
    return std::move(pgrp);
}

/**
 * Sets the groups and adds new elements to the layout.
 * @param horizontalGroup the horizontal alignment group
 * @param verticalGroup the vertical alignment group
 * @param elements the GUIElements the groups refer to
 */
void GroupLayout::SetGroups(std::unique_ptr<grplayout::sequential_group> horizontalGroup,
        std::unique_ptr<grplayout::sequential_group> verticalGroup, ElementList elements)
{
    columnSizes.clear();
    columnPositions.clear();
    guiElements.clear();
    std::vector<glm::uvec2> gridPositions(elements.size(), glm::uvec2(0, 0));
    std::vector<glm::i8vec2> gridAlign(elements.size(), glm::i8vec2(1, 1));
    float colPos = 0.0f;
    for (unsigned int i = 0; i < horizontalGroup->elements.size(); ++i) {
        grplayout::sequential_group::group_element& grpElement = horizontalGroup->elements[i];
        columnPositions.push_back(colPos);
        float maxWidth = grpElement->GetMaxWidth(elements);
        columnSizes.push_back(maxWidth + 2 * dist);
        colPos += maxWidth;
        for (unsigned int id : grpElement->elementIds) {
            gridPositions[id].x = i;
            gridAlign[id].x = grpElement->align;
        }
    }

    rowSizes.clear();
    rowPositions.clear();
    float rowPos = 0.0f;
    for (unsigned int j = 0; j < verticalGroup->elements.size(); ++j) {
        grplayout::sequential_group::group_element& grpElement = verticalGroup->elements[j];
        rowPositions.push_back(rowPos);
        float maxHeight = grpElement->GetMaxHeight(elements);
        rowSizes.push_back(maxHeight + 2 * dist);
        rowPos += maxHeight;
        for (unsigned int id : grpElement->elementIds) {
            gridPositions[id].y = j;
            gridAlign[id].y = grpElement->align;
        }
    }

    ResizeGrid();
    for (unsigned int id = 0; id < elements.size(); ++id) {
        AddElement(gridPositions[id], std::move(elements[id]), gridAlign[id]);
    }
}
