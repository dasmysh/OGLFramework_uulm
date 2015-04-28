/**
 * @file   gui_info.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the definition of gui info objects.
 */

#ifndef GUI_INFO_H
#define GUI_INFO_H

#include "main.h"

class Font;
class GPUProgram;
class GLTexture2D;
class SimpleQuadRenderable;
class GUIRenderable;

namespace guiinfo
{

    struct font_info
    {
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float fontSize = 15.0f;
        float fontWeight = 1.0f;
        float fontShearing = 0.0f;
        Font* font = nullptr;
        GPUProgram* fontProgram = nullptr;
    };

    struct box_info
    {
        glm::vec2 position = glm::vec2(0.0f, 0.0f);
        glm::vec2 size = glm::vec2(0.0f, 0.0f);
    };

    struct element_info
    {
        glm::vec2 size = glm::vec2(0.0f, 0.0f);
        box_info box[2] = {};
    };

    struct button_info
    {
        font_info* fontInfo = nullptr;
        GLTexture2D* themeTexture = nullptr;
        GPUProgram* guiProgram = nullptr;
        glm::vec2 textRel = glm::vec2(0.0f, 0.0f);
        float height = 0.0f;
        element_info left = {};
        element_info right = {};
    };

    struct input_text_info
    {
        font_info* fontInfo = nullptr;
        GLTexture2D* themeTexture = nullptr;
        GPUProgram* guiProgram = nullptr;
        SimpleQuadRenderable* cursor = nullptr;
        glm::vec2 textRel = glm::vec2(0.0f, 0.0f);
        float height = 0.0f;
        element_info left = {};
        element_info right = {};
    };

    struct slider_info
    {
        font_info* fontInfo = nullptr;
        GLTexture2D* themeTexture = nullptr;
        GPUProgram* guiProgram = nullptr;
        std::array<GUIRenderable*, 2> marker = {
            {nullptr, nullptr}
        };
        glm::vec2 textRel = glm::vec2(0.0f, 0.0f);
        float height = 0.0f;
        element_info left = {};
        element_info right = {};
    };

    struct checkbox_info
    {
        font_info* fontInfo = nullptr;
        GLTexture2D* themeTexture = nullptr;
        GPUProgram* guiProgram = nullptr;
        std::array<GUIRenderable*, 2> checker = {
            {nullptr, nullptr}};
        std::array<GUIRenderable*, 2> checkerChecked = {
            {nullptr, nullptr}};
        float height = 0.0f;
        glm::vec2 textRel = glm::vec2(0.0f, 0.0f);
    };

    struct radio_button_info
    {
        font_info* fontInfo = nullptr;
        GLTexture2D* themeTexture = nullptr;
        GPUProgram* guiProgram = nullptr;
        std::array<GUIRenderable*, 2> checker = {
            {nullptr, nullptr}};
        std::array<GUIRenderable*, 2> checkerChecked = {
            {nullptr, nullptr}};
        float height = 0.0f;
        glm::vec2 textRel = glm::vec2(0.0f, 0.0f);
    };
}

#endif /* GUI_INFO_H */
