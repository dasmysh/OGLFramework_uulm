/**
 * @file   GroupLayout.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the definition of GroupLayout.
 */

#ifndef GROUPLAYOUT_H
#define GROUPLAYOUT_H

#include "GUIElement.h"
#include "GridLayout.h"

namespace grplayout
{

    /** Contains all GUIElements that are parallel in one row/column and their alignment. */
    struct parallel_group
    {
        /** The elements alignment. */
        glm::i8 align = 1;
        /** The ids of the elements contained. */
        std::vector<unsigned int> elementIds = {};

        parallel_group* AddElement(unsigned int elementId);
        float GetMaxWidth(const std::vector<std::unique_ptr<GUIElement> >& elements);
        float GetMaxHeight(const std::vector<std::unique_ptr<GUIElement> >& elements);
    };

    /** Contains multiple parallel_group objects. */
    struct sequential_group
    {
        /** The element type of the group. */
        using group_element = std::unique_ptr<parallel_group>;
        /** The elements of the group. */
        std::vector<group_element> elements = {};

        sequential_group* AddElement(unsigned int elementId);
        sequential_group* AddGroup(std::unique_ptr<parallel_group> group);
    };
}

/**
 * @brief  Layout that depends on the GridLayout. The grid is determindes by a list of horizontal and vertical
 * groups of elements.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 */
class GroupLayout : public GridLayout
{
public:
    GroupLayout(float distance = 5.0f);
    virtual ~GroupLayout() = default;
    /** Deleted copy constructor. */
    GroupLayout(const GroupLayout& orig) = delete;
    /** Deleted copy assignment operator. */
    GroupLayout& operator=(const GroupLayout& orig) = delete;
    /** Default move constructor. */
    GroupLayout(GroupLayout&& orig) = default;
    /** Default move assignment operator. */
    GroupLayout& operator=(GroupLayout&& orig) = default;

    std::unique_ptr<grplayout::sequential_group> CreateSequentialGroup();
    std::unique_ptr<grplayout::parallel_group> CreateParallelGroup(glm::i8 align);
    void SetGroups(std::unique_ptr<grplayout::sequential_group> horizontalGroup,
            std::unique_ptr<grplayout::sequential_group> verticalGroup, ElementList elements);

private:

};

#endif /* GROUPLAYOUT_H */
