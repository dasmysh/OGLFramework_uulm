/**
 * @file   GUIElementSizeable.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   27. Februar 2014
 *
 * @brief  Contains the implementation of GUIElementSizeable.
 */

#include "GUIElementSizeable.h"
#include "gfx/Vertices.h"

GUIElementSizeable::GUIElementSizeable(const glm::vec3& relPos) :
GUIElement(relPos, glm::vec2(0.0f, 0.0f))
{
}

void GUIElementSizeable::InitializeSize(const glm::vec2& compSize, const guiinfo::element_info& left,
        const guiinfo::element_info& right, GPUProgram* guiProgram)
{
    float ctrWidth = compSize.x - left.size.y - right.size.y;
    std::vector<GUIVertex> vertices[2];
    for (unsigned int i = 0; i < 2; ++i) {
        vertices[i].resize(8);
        vertices[i][0].pos = glm::vec3(0.0f, 0.0f, 0.0f);
        vertices[i][0].texCoords = glm::vec2(left.box[i].position.x, left.box[i].position.y);
        vertices[i][1].pos = glm::vec3(0.0f, left.size.y, 0.0f);
        vertices[i][1].texCoords = glm::vec2(left.box[i].position.x,
                left.box[i].position.y + left.box[i].size.y);
        vertices[i][2].pos = glm::vec3(left.size.x, 0.0f, 0.0f);
        vertices[i][2].texCoords = glm::vec2(left.box[i].position.x,
                left.box[i].position.y + left.box[i].size.y);
        vertices[i][3].pos = glm::vec3(left.size.x, left.size.y, 0.0f);
        vertices[i][3].texCoords = glm::vec2(left.box[i].position.x + left.box[i].size.x,
                left.box[i].position.y + left.box[i].size.y);
        vertices[i][4].pos = glm::vec3(left.size.x + ctrWidth, 0.0f, 0.0f);
        vertices[i][4].texCoords = glm::vec2(right.box[i].position.x, right.box[i].position.y);
        vertices[i][5].pos = glm::vec3(left.size.x + ctrWidth, right.size.y, 0.0f);
        vertices[i][5].texCoords = glm::vec2(right.box[i].position.x,
                right.box[i].position.y + right.box[i].size.y);
        vertices[i][6].pos = glm::vec3(left.size.x + ctrWidth + right.size.x, 0.0f, 0.0f);
        vertices[i][6].texCoords = glm::vec2(right.box[i].position.x + right.box[i].size.x,
                right.box[i].position.y);
        vertices[i][7].pos = glm::vec3(left.size.x + ctrWidth + right.size.x, right.size.y, 0.0f);
        vertices[i][7].texCoords = glm::vec2(right.box[i].position.x + right.box[i].size.x,
                right.box[i].position.y + right.box[i].size.y);
    }
    std::vector<unsigned int> indices{0, 1, 2, 2, 1, 3, 2, 3, 4, 4, 3, 5, 4, 5, 6, 6, 5, 7};

    renderable.reset(new GUIRenderable(vertices[0], indices, guiProgram));
    renderableFocused.reset(new GUIRenderable(vertices[1], indices, guiProgram));

    SetSize(glm::vec2(compSize.x, compSize.y));
}

void GUIElementSizeable::RenderGeometry(GLBatchRenderTarget& rt)
{
    rt.DrawGUIGeometry(focus ? renderableFocused.get() : renderable.get());
}
