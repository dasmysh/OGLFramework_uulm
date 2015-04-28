/**
 * @file   font_metrics.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   6. Februar 2014
 *
 * @brief  Contains structures describing a font.
 */

#ifndef FONT_METRICS_H
#define FONT_METRICS_H

#include "main.h"

/** Describes a font texture page. */
struct font_page
{
    /** Holds the font pages id. */
    unsigned int id;
    /** Holds the font pages file name. */
    std::string filename;
};

/** Describes the properties of a glyph needed in a shader. */
struct glyph_metrics
{
    // TODO: formulas:
    // tdim: texture dimensions (get by textureSize in shader)
    // th, tw: texture coordinate height/width
    // hp, wp: height/width in pixels
    // hs, ws: height/width on screen
    // ap: aspect ratio in pixels
    // hn, wn: height/width normalized
    // save: th, hp, ap => tw = (hp*ap) / tdim.x
    //                  => wp = hp*ap
    //                  => xn = xp*normfactor
    //                  => xs = xp*normfactor*fontSize
    // => th, hp, ap in buffer => wp, tp
    // => normfactor*fontsize in uniform => xs
    /** The position of the glyph in the texture. */
    glm::vec2 pos;
    /** The rendering offsets of the glyph in pixels. */
    glm::vec2 off;
    /** The width of the glyph in the texture coordinates. */
    float heightInTex;
    /** The width of the glyph in pixels. */
    float heightInPixels;
    /** The aspect ratio of the glyphs dimensions. */
    float aspectRatio;
    /** The texture page the glyph is on. */
    float page;
};

/** Describes the properties of a glyph. */
struct font_glyph
{
    /** The character of the glyph. */
    char id;
    /** The glyphs metrics. */
    glyph_metrics metrics;
    /** The distance to the next glyph in a text. */
    float xadv;
};

/** Describes the properties of a font. */
struct font_metrics
{
    /** The texture pages the font uses. */
    std::vector<font_page> pages;
    /** The fonts character descriptions. */
    std::vector<font_glyph> chars;
    /** The fonts base line. */
    float baseLine;
    /** The font size normalization factor (1/fontSizeFromFile). */
    float sizeNormalization;
};

#endif /* FONT_METRICS_H */
