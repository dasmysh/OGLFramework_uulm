/**
 * @file   GridLayout.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the definition of GridLayout.
 */

#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H

#include "GUILayout.h"

/**
 * @brief  The GridLayout implements the GUILayout as a grid. The grid consists of sizable columns and rows
 * That can contain a single GUIElement each.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 */
class GridLayout : public GUILayout
{
public:
    GridLayout(unsigned int columns, unsigned int rows, float distance = 5.0f);
    GridLayout(std::initializer_list<float> colSizes,
            std::initializer_list<float> rwSizes, float distance = 5.0f);
    GridLayout(const glm::vec2& size, std::initializer_list<float> colSizes,
            std::initializer_list<float> rwSizes, float distance = 5.0f);
    virtual ~GridLayout() = default;
    /** Deleted copy constructor. */
    GridLayout(const GridLayout& orig) = delete;
    /** Deleted copy assignment operator. */
    GridLayout& operator=(const GridLayout& orig) = delete;
    /** Default move constructor. */
    GridLayout(GridLayout&& orig) = default;
    /** Default move assignment operator. */
    GridLayout& operator=(GridLayout&& orig) = default;

    virtual bool HandleMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
            int nMouseWheelDelta, const glm::vec2& pos) override;
    void AddElement(const glm::uvec2& gridPos, std::unique_ptr<GUIElement> element,
            const glm::i8vec2& align = glm::i8vec2(1, 1));

protected:
    /** Holds the column sizes. */
    std::vector<float> columnSizes = {};
    /** Holds the row sizes. */
    std::vector<float> rowSizes = {};
    /** Holds the column positions. */
    std::vector<float> columnPositions = {};
    /** Holds the row positions. */
    std::vector<float> rowPositions = {};
    /** Holds the distance of elements to the grids borders. */
    float dist;

    virtual void RecalculateRelative() override;
    void ResizeGrid();

private:
    template<typename T>
    using GridType = std::vector<std::vector<T> >;
    /** Holds the grids contents. */
    GridType<GUIElement*> grid = {};
    /** Holds the alignment of the grids contents. */
    GridType<glm::i8vec2> alignGrid = {};

    void CalculateRelative(const glm::uvec2& gridPos);

};

#endif /* GRIDLAYOUT_H */
