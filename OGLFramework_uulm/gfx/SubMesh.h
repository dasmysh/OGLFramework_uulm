/**
 * @file   SubMesh.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.13
 *
 * @brief  Contains the sub-mesh class.
 */

#ifndef SUBMESH_H
#define SUBMESH_H

#include "SubMeshMaterialChunk.h"
#include "FreeFormObjects.h"
#include <set>
#include <boost/geometry.hpp>
#include "core/math/math.h"

namespace cgu {

    /** Contains vertex connectivity information. */
    struct MeshConnectVertex
    {
        /** Holds the vertex index. */
        unsigned int idx;
        /** Holds the vertexes triangles. */
        std::vector<unsigned int> triangles;
    };

    /** Contains indices for triangles vertices and connectivity. */
    struct MeshConnectTriangle
    {
        explicit MeshConnectTriangle(const std::array<unsigned int, 3>& v) :
            vertex({ { v[0], v[1], v[2] } }),
            neighbors({ { -1, -1, -1 } })
        {};

        bool operator<(const MeshConnectTriangle& rhs) const {
            if (vertex[0] < rhs.vertex[0]) return true;
            else if (vertex[0] == rhs.vertex[0]) {
                if (vertex[1] < rhs.vertex[1]) return true;
                else if (vertex[1] == rhs.vertex[1]) {
                    if (vertex[2] < rhs.vertex[2]) return true;
                }
            }
            return false;
        };

        bool operator==(const MeshConnectTriangle& rhs) const {
            return rhs.vertex[0] == vertex[0]
                && rhs.vertex[1] == vertex[1]
                && rhs.vertex[2] == vertex[2];
        };

        /** Holds the triangles vertices. */
        std::array<unsigned int, 3> vertex;
        /** Holds the triangles face vertices (not only position). */
        mutable std::array<unsigned int, 3> faceVertex;
        /** Holds the triangles neighbors. */
        std::array<int, 3> neighbors;
    };

    /**
     * Used for counting statements in an .obj file.
     * @internal
     */
    struct ObjCountState
    {
        ObjCountState();
        void ResetSubMesh();

        /** Holds the current vertex count. */
        unsigned int numVerts;
        /** Holds the current texture coordinates count. */
        unsigned int numTexCoords;
        /** Holds the current normals count. */
        unsigned int numNormals;
        /** Holds the current parameter vertex count. */
        unsigned int numParameterVerts;
        /** Holds the number of points. */
        unsigned int numPoints;
        /** Holds the number of line indices (per sub-mesh). */
        unsigned int numLineIndices;
        /** Holds the number of face indices (per sub-mesh). */
        unsigned int numFaceIndices;
        /** Holds the number of curves on a surface (per sub-mesh). */
        unsigned int numCurv2;
        /** Holds the number of curves (per sub-mesh). */
        unsigned int numCurv;
        /** Holds the number of surfaces (per sub-mesh). */
        unsigned int numSurf;
        /** Holds the number of materials (per sub-mesh). */
        unsigned int numMtls;
    };

    /**
     * A SubMesh is a sub group of geometry in a mesh. It does not have its own
     * vertex information but uses indices to define which vertices of the mesh are used.
     */
    class SubMesh
    {
    public:
        SubMesh();
        explicit SubMesh(const std::string& name);
        SubMesh(const SubMesh&);
        SubMesh& operator=(const SubMesh&);
        SubMesh(SubMesh&&);
        SubMesh& operator=(SubMesh&&);
        virtual ~SubMesh();

        void ReserveSubMesh(ObjCountState& countState);
        void FinishMaterial(SubMeshMaterialChunk& matChunk);

        /** Holds the sub-meshes object name. */
        std::string objectName;
        /** Holds the SubMeshMaterialChunk objects used in a SubMesh. */
        std::vector<SubMeshMaterialChunk> mtlChunks;
        /** Holds whether the lines in this sub-mesh use texture coordinates. */
        bool lineHasTexture;
        /** Holds whether the faces in this sub-mesh use texture coordinates. */
        bool faceHasTexture;
        /** Holds whether the faces in this sub-mesh use normal vectors. */
        bool faceHasNormal;
        /** Holds the list of indices to points in this sub-mesh. */
        std::vector<unsigned int> pointIndices;
        /** Holds the list of indices to end-points of line segments in this sub-mesh (as a line-list). */
        std::vector<unsigned int> lineIndices;
        /** Holds the list of indices to faces (triangles) in this sub-mesh (as a triangle-list). */
        std::vector<unsigned int> faceIndices;
        /** Holds the curves in this sub-mesh. */
        std::vector<curv> curves;
        /** Holds the curves on surfaces in this sub-mesh. */
        std::vector<curv2> curv2es;
        /** Holds the surfaces in this sub-mesh. */
        std::vector<surf> surfaces;

        /** Holds a list of face (triangle) vertices (location only) to be used for
         *  later calculation of triangle connectivity. */
        std::set<MeshConnectTriangle> trianglePtsIndices;
        /** Holds the first triangle index. */
        unsigned int firstTriIndex;
        /** Holds the number of triangles in this sub-mesh. */
        unsigned int numTriangles;
        /** Holds the bounding box of this sub-mesh. */
        cguMath::AABB3<float> aabb;

    protected:
        typedef boost::geometry::model::point<float, 3, boost::geometry::cs::cartesian> point;
        typedef boost::geometry::model::box<point> box;
        typedef boost::geometry::model::polygon<point, false, false> polygon; // CCW, open polygon
        typedef std::pair<box, unsigned> polyIdxBox;

        typedef boost::geometry::index::rtree<polyIdxBox, boost::geometry::index::quadratic<16>> RTreeType;

    public:
        /** Holds the tree for fast finding points in triangles. */
        RTreeType fastFindTree;
    };
}

#endif /* SUBMESH_H */
