/**
 * @file   SubMeshMaterialChunk.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 *
 * @brief Contains descriptions of parts of submeshes that share a material.
 */

#ifndef SUBMESHMATERIALCHUNK_H
#define SUBMESHMATERIALCHUNK_H

#include "Material.h"

namespace cgu {

    class SubMesh;

    /**
     * Describes a part of a submesh that shares a material.
     * Does not contain real mesh data, only index-buffer sequences are defined here.
     */
    class SubMeshMaterialChunk
    {
    public:
        /** Default constructor. */
        SubMeshMaterialChunk() :
            material(nullptr),
            point_seq_begin(0),
            point_seq_num(0),
            line_seq_begin(0),
            line_seq_num(0),
            face_seq_begin(0),
            face_seq_num(0)
        {
        }

        /**
         * Constructor.
         * @param mat the material to use
         */
        explicit SubMeshMaterialChunk(const Material* mat) :
            material(mat),
            point_seq_begin(0),
            point_seq_num(0),
            line_seq_begin(0),
            line_seq_num(0),
            face_seq_begin(0),
            face_seq_num(0)
        {
        }

        /**
         * Constructor.
         * Builds a SubMeshMaterialChunk using the previous chunk and a material.
         * @param prevChunk the previous chunk
         * @param mat the material
         */
        SubMeshMaterialChunk(const SubMeshMaterialChunk& prevChunk, const Material* mat) :
            material(mat),
            point_seq_begin(prevChunk.point_seq_begin + prevChunk.point_seq_num),
            point_seq_num(0),
            line_seq_begin(prevChunk.line_seq_begin + prevChunk.line_seq_num),
            line_seq_num(0),
            face_seq_begin(prevChunk.face_seq_begin + prevChunk.face_seq_num),
            face_seq_num(0)
        {
        }

        /**
         * Finishes the chunk using the current index-buffer size (= first index of next chunk).
         * @param nextPointIdx the next point index
         * @param nextLineIdx the next line index
         * @param nextFaceIdx the next face index
         */
        void Finish(unsigned int nextPointIdx, unsigned int nextLineIdx, unsigned int nextFaceIdx)
        {
            // TODO: take mesh .. add to it
            point_seq_num = nextPointIdx - point_seq_begin;
            line_seq_num = nextLineIdx - line_seq_begin;
            face_seq_num = nextFaceIdx - face_seq_begin;
        }

        /** Checks if the chunk is empty. */
        bool IsEmpty() const
        {
            return point_seq_num == 0 && line_seq_num == 0 && face_seq_num == 0;
        }

        /** Holds the chunks material. */
        const Material* material;
        /** Holds the chunks point index sequence start. */
        unsigned int point_seq_begin;
        /** Holds the chunks point index sequence size. */
        unsigned int point_seq_num;
        /** Holds the chunks line index sequence start. */
        unsigned int line_seq_begin;
        /** Holds the chunks line index sequence size. */
        unsigned int line_seq_num;
        /** Holds the chunks face index sequence start. */
        unsigned int face_seq_begin;
        /** Holds the chunks face index sequence size. */
        unsigned int face_seq_num;
    };
}

#endif /* SUBMESHMATERIALCHUNK_H */
