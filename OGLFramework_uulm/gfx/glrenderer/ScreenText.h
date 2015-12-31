/**
 * @file   ScreenText.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.02.04
 *
 * @brief  Contains the definition of ScreenText.
 */

#ifndef SCREENTEXT_H
#define SCREENTEXT_H

#include "main.h"
#include "gfx/glrenderer/GLVertexAttributeArray.h"

namespace cgu {

    class GPUProgram;
    class GPUProgramManager;
    class Font;

    /**
     * @brief  Displays text on the screen.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.02.04
     */
    class ScreenText
    {
    public:
        ScreenText(const Font& fnt, GPUProgram* fontProg, const std::string& txt,
            const glm::vec2& pos, const glm::vec2& dir, const glm::vec2& fntSize = glm::vec2(10.0f, 10.0f),
            float fntWeight = 1.0f, float fntShearing = 0.0f, float depth = 0.0f);
        ScreenText(const Font& fnt, GPUProgram* fontProg, const std::string& txt,
            const glm::vec2& pos, const glm::vec2& dir, float fntSize = 10.0f,
            float fntWeight = 1.0f, float fntShearing = 0.0f, float depth = 0.0f);
        ScreenText(const Font& fnt, GPUProgram* fontProg, const std::string& txt,
            const glm::vec2& pos, float fntSize = 10.0f, float fntWeight = 1.0f,
            float fntShearing = 0.0f, float depth = 0.0f);
        ScreenText(const ScreenText&);
        ScreenText& operator=(const ScreenText&);
        ScreenText(ScreenText&&);
        ScreenText& operator=(ScreenText&&);
        virtual ~ScreenText();

        void SetText(const std::string& txt, bool reinit = true);
        void SetPosition(const glm::vec2& pos);
        void SetDirection(const glm::vec2& dir, bool reinit = true);
        void SetFontSize(const glm::vec2& size, bool reinit = true);
        void SetFontSize(float size, bool reinit = true);
        void SetDepthLayer(float depth, bool reinit = true);
        void SetFontWeight(float weight);
        void SetFontShearing(float shearing);
        void SetColor(const glm::vec4& col);

        void Draw();
        void DrawMultiple();
        float GetPixelLength() const;
        float GetBaseLine() const;

    private:
        /** Holds the font. */
        const Font* font;
        /** Holds the boldness of the font. */
        float fontWeight;
        /** Holds the shearing of the font. */
        float fontShearing;
        /** Holds the size of the font. */
        glm::vec2 fontSize;
        /** Holds the string to render. */
        std::string text;
        /** Holds the position to render to. */
        glm::vec2 position;
        /** Holds the direction of the text. */
        glm::vec2 direction;
        /** Holds the text color. */
        glm::vec4 color;
        /** Holds the depth layer for this element. */
        float depthLayer;
        /** Holds the VBO chain for the text. */
        std::vector<GLuint> textVBOs;
        /** Holds the font VBO fences. */
        std::vector<GLsync> textVBOFences;
        /** Holds the current sizes of the buffers. */
        std::vector<unsigned int> textVBOSizes;
        /** Holds the currently used buffer. */
        unsigned int currentBuffer;
        /** Holds the font rendering GPU program. */
        GPUProgram* fontProgram;
        /** Holds the vertex attribute positions. */
        std::vector<BindingLocation> vertexAttribPos;
        /** Holds the vertex attribute bindings for the font shader. */
        VertexAttributeBindings attribBind;
        /**
         * Holds the uniform names for the
         * boldness/shearing/scalex/scaley uniform([0])
         * the posxy/dirxy uniform([1])
         * the color ([2])
         * and the font texture([3]).
         */
        std::vector<BindingLocation> uniformNames;
        /** Holds the binding location for the font metrics uniform buffer. */
        BindingLocation fontMetricsBindingLocation;
        /** Holds the current length of the string in screen space pixels. */
        float pixelLength;

        void InitializeText(bool first = false);
        void Initialize();
    };
}

#endif /* SCREENTEXT_H */
