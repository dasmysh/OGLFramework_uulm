/**
 * @file   GridLayout.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   18. Februar 2014
 *
 * @brief  Contains the implementation of GridLayout.
 */

#include "GridLayout.h"

/**
 * Constructor.
 * The grids size will be divided equally for all rows/columns
 * @param columns the number of columns in the grid
 * @param rows the number of rows in the grid
 * @param distance the distance between the grids borders and an element
 */
GridLayout::GridLayout(unsigned int columns, unsigned int rows, float distance) :
GUILayout(glm::vec2(0.0f, 0.0f), glm::vec2(-1.0f, -1.0f)),
columnPositions(columns),
rowPositions(rows),
dist(distance),
grid(columns, std::vector<GUIElement*>(rows, nullptr)),
alignGrid(columns, std::vector<glm::i8vec2>(rows, glm::i8vec2(1, 1)))
{
    columnSizes.resize(columns, -size.x / static_cast<float> (columns));
    rowSizes.resize(rows, -size.y / static_cast<float> (rows));

    float colPos = 0.0f;
    for (unsigned int i = 0; i < columnSizes.size(); ++i) {
        columnPositions[i] = colPos;
        colPos += columnSizes[i];
    }

    float rowPos = 0.0f;
    for (unsigned int i = 0; i < rowSizes.size(); ++i) {
        rowPositions[i] = rowPos;
        rowPos += rowSizes[i];
    }
}

/**
 * Constructor.
 * @param colSizes the grids columns sizes
 * @param rwSizes the grids rows sizes
 * @param distance the distance between the grids borders and an element
 */
GridLayout::GridLayout(std::initializer_list<float> colSizes,
        std::initializer_list<float> rwSizes, float distance) :
GridLayout(glm::vec2(-1.0f, -1.0f), colSizes, rwSizes, distance)
{
}

/**
 * Constructor.
 * @param size the grids size
 * @param colSizes the grids columns sizes
 * @param rwSizes the grids rows sizes
 * @param distance the distance between the grids borders and an element
 */
GridLayout::GridLayout(const glm::vec2& size, std::initializer_list<float> colSizes,
        std::initializer_list<float> rwSizes, float distance) :
GUILayout(glm::vec2(0.0f, 0.0f), size),
columnSizes(colSizes),
rowSizes(rwSizes),
columnPositions(colSizes.size()),
rowPositions(rwSizes.size()),
dist(distance),
grid(colSizes.size(), std::vector<GUIElement*>(rwSizes.size(), nullptr)),
alignGrid(colSizes.size(), std::vector<glm::i8vec2>(rwSizes.size(), glm::i8vec2(1, 1)))
{
    float colPos = 0.0f;
    for (unsigned int i = 0; i < columnSizes.size(); ++i) {
        columnPositions[i] = colPos;
        colPos += columnSizes[i];
    }

    float rowPos = 0.0f;
    for (unsigned int i = 0; i < rowSizes.size(); ++i) {
        rowPositions[i] = rowPos;
        rowPos += rowSizes[i];
    }
}

/**
 * Adds a new element to the grid. The grids position needs to be empty for that.
 * @param gridPos the position in the grid to add the element to
 * @param element the element to add
 * @param align the elements align
 */
void GridLayout::AddElement(const glm::uvec2& gridPos, std::unique_ptr<GUIElement> element,
        const glm::i8vec2& align)
{
    assert(grid[gridPos.x][gridPos.y] == nullptr);
    grid[gridPos.x][gridPos.y] = element.get();
    alignGrid[gridPos.x][gridPos.y] = align;

    CalculateRelative(gridPos);

    guiElements.push_back(std::move(element));
}

void GridLayout::RecalculateRelative()
{
    glm::uvec2 gridPos(0, 0);
    for (; gridPos.x < columnSizes.size(); ++gridPos.x) {
        for (; gridPos.y < rowSizes.size(); ++gridPos.y) {
            CalculateRelative(gridPos);
        }
    }
}

bool GridLayout::HandleMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
        int nMouseWheelDelta, const glm::vec2& pos)
{
    float colBegin = 0.0f, rowBegin = 0.0f;
    unsigned int i = 0, j = 0;
    for (; i < columnSizes.size(); ++i) {
        float colSize = columnSizes[i] > 0.0f ? columnSizes[i] : size.x * columnSizes[i];
        if (pos.x >= colBegin && pos.x <= colBegin + colSize) {
            break;
        }
    }

    for (; j < rowSizes.size(); ++j) {
        float rowSize = rowSizes[j] > 0.0f ? rowSizes[j] : size.y * rowSizes[j];
        if (pos.y >= rowBegin && pos.y <= rowBegin + rowSize) {
            break;
        }
    }

    if (grid[i][j]->IsContained(pos)) {
        FocusElement(grid[i][j]);
        return grid[i][j]->HandleMouse(bLeftButtonDown, bRightButtonDown, bMiddleButtonDown,
                nMouseWheelDelta, pos);
    }
    return false;
}

/**
 * Calculates the relative (and absolute) positions of an element in the grid.
 * @param gridPos the position of the element in the grid
 */
void GridLayout::CalculateRelative(const glm::uvec2& gridPos)
{
    glm::vec2 relPos(dist, dist);
    glm::vec2 contSize(columnSizes[gridPos.x], rowSizes[gridPos.y]);
    GUIElement* element = grid[gridPos.x][gridPos.y];
    if (element == nullptr) return;
    if (alignGrid[gridPos.x][gridPos.y].x == 0) {
        relPos.x = (contSize.x - element->GetSize().x) / 2.0f;
    } else if (alignGrid[gridPos.x][gridPos.y].x < 0) {
        relPos.x = -relPos.x;
    }

    if (alignGrid[gridPos.x][gridPos.y].y == 0) {
        relPos.y = (contSize.y - element->GetSize().y) / 2.0f;
    } else if (alignGrid[gridPos.x][gridPos.y].y < 0) {
        relPos.y = -relPos.y;
    }

    element->SetRelativePosition(relPos);
    element->CalculateAbsolute(glm::vec2(columnPositions[gridPos.x], rowPositions[gridPos.y]), contSize);
}

void GridLayout::ResizeGrid()
{
    grid.resize(columnSizes.size(), std::vector<GUIElement*>(rowSizes.size(), nullptr));
    alignGrid.resize(columnSizes.size(), std::vector<glm::i8vec2>(rowSizes.size(), glm::i8vec2(1, 1)));
}
