/**
 * @file   Font.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   6. Februar 2014
 *
 * @brief  Contains the definition of Font.
 */

#ifndef FONT_H
#define FONT_H

#include "../font_metrics.h"
#include "GLVertexAttributeArray.h"

namespace cgu {

    class GPUProgram;
    class GLUniformBuffer;
    class GLTexture;

    /**
     * @brief  Represents a font.
     * This font engine uses a modified BMFont XML-Format (<a href="http://www.angelcode.com/products/bmfont/doc/file_format.html">BMFont Format</a>.
     * The fonts used need to be converted into signed distance fields.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   6. Februar 2014
     */
    class Font : public Resource
    {
    private:
        /** Deleted copy constructor. */
        Font(const Font& orig) = delete;
        /** Deleted copy assignment operator. */
        Font& operator=(const Font&) = delete;

    public:
        Font(const std::string& fontName, ApplicationBase* app);
        virtual ~Font();
        /** Default move constructor. */
        Font(Font&& orig);
        /** Default move assignment operator. */
        Font& operator=(Font&& orig);

        virtual void Load() override;
        virtual void Unload() override;

        void UseFont(GPUProgram* fontProgram, BindingLocation fontMetricsLocation) const;
        const font_metrics& GetFontMetrics() const;
        unsigned int GetCharacterId(char character) const;

    private:
        /** Holds the font texture. */
        std::unique_ptr<GLTexture> fontPages;
        /** Holds the font metrics. */
        font_metrics fm;
        /** Holds the font metrics uniform buffer. */
        std::unique_ptr<GLUniformBuffer> fontMetrics;
        /** Holds the binding point for the font metrics buffer. */
        GLuint fontMetricsBindingPoint;

        void UnloadLocal();
    };
}

#endif /* FONT_H */
