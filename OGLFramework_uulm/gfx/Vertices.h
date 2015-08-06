/**
 * @file   Vertices.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 *
 * @brief  Contains all vertices used for rendering.
 */

#ifndef VERTICES_H
#define VERTICES_H

#include "../main.h"

namespace cgu {

    /** Represents a vertex of a line. */
    struct LineVertex
    {
        /** Holds the vertices position. */
        glm::vec3 pos;
        /** Holds the vertices texture coordinates. */
        glm::vec2 tex;

        /**
         * Comparison operator for equality.
         * @param rhs the other LineVertex to compare to
         * @return whether the vertices are equal
         */
        bool operator==(const LineVertex& rhs)
        {
            return pos == rhs.pos && tex == rhs.tex;
        }
    };

    /** Represents a vertex of a face. */
    struct FaceVertex
    {
        /** Holds the vertices position. */
        glm::vec3 pos;
        /** Holds the vertices texture coordinates. */
        glm::vec2 tex;
        /** Holds the vertices normal. */
        glm::vec3 normal;

        /**
         * Comparison operator for equality.
         * @param rhs the other FaceVertex to compare to
         * @return whether the vertices are equal
         */
        bool operator==(const FaceVertex& rhs)
        {
            return pos == rhs.pos && tex == rhs.tex && normal == rhs.normal;
        }
    };

    /** Represents a vertex of a text character. */
    struct FontVertex
    {
        /** Holds the characters position. */
        glm::vec3 pos;
        /** Holds the character index to render. */
        unsigned int idx;

        /**
         * Comparison operator for equality.
         * @param rhs the other FontVertex to compare to
         * @return whether the vertices are equal
         */
        bool operator==(const FontVertex& rhs)
        {
            return pos == rhs.pos && idx == rhs.idx;
        }
    };

    /** Represents a vertex of a gui element. */
    struct GUIVertex
    {
        /** Holds the vertices position. */
        glm::vec3 pos;
        /** Holds the vertices texture coordinates. */
        glm::vec2 texCoords;

        /**
         * Comparison operator for equality.
         * @param rhs the other GUIVertex to compare to
         * @return whether the vertices are equal
         */
        bool operator==(const GUIVertex& rhs)
        {
            return pos == rhs.pos && texCoords == rhs.texCoords;
        }
    };
}

#endif /* VERTICES_H */
