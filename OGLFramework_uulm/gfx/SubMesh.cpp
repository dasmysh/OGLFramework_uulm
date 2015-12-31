/**
 * @file   SubMesh.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.13
 *
 * @brief  Implementation of the SubMesh.
 */

#include "SubMesh.h"

namespace cgu {

    /** Default constructor. */
    ObjCountState::ObjCountState() :
        numVerts(0),
        numTexCoords(0),
        numNormals(0),
        numParameterVerts(0),
        numPoints(0),
        numLineIndices(0),
        numFaceIndices(0),
        numCurv2(0),
        numCurv(0),
        numSurf(0),
        numMtls(0)
    {
    }

    /**
     * Resets the count state values for sub-meshes.
     */
    void ObjCountState::ResetSubMesh()
    {
        numPoints = 0;
        numLineIndices = 0;
        numFaceIndices = 0;
        numCurv = 0;
        numCurv2 = 0;
        numSurf = 0;
        numMtls = 0;
    }

    /** Constructor. */
    SubMesh::SubMesh() :
        objectName(""),
        lineHasTexture(false),
        faceHasTexture(false),
        faceHasNormal(false),
        firstTriIndex(0),
        numTriangles(0)
    {
    }

    /** Default destructor. */
    SubMesh::~SubMesh() = default;

    /**
     * Constructor.
     * @param name the sub-meshes name
     */
    SubMesh::SubMesh(const std::string& name) :
        objectName(name),
        lineHasTexture(false),
        faceHasTexture(false),
        faceHasNormal(false),
        firstTriIndex(0),
        numTriangles(0)
    {
    }

    /** Default copy constructor. */
    SubMesh::SubMesh(const SubMesh&) = default;
    /** Default copy assignment operator. */
    SubMesh& SubMesh::operator=(const SubMesh&) = default;

    /** Default move constructor. */
    SubMesh::SubMesh(SubMesh&& rhs) :
        objectName(std::move(rhs.objectName)),
        mtlChunks(std::move(rhs.mtlChunks)),
        lineHasTexture(rhs.lineHasTexture),
        faceHasTexture(rhs.faceHasTexture),
        faceHasNormal(rhs.faceHasNormal),
        pointIndices(std::move(rhs.pointIndices)),
        lineIndices(std::move(rhs.lineIndices)),
        curves(std::move(rhs.curves)),
        curv2es(std::move(rhs.curv2es)),
        surfaces(std::move(rhs.surfaces)),
        trianglePtsIndices(std::move(rhs.trianglePtsIndices)),
        firstTriIndex(rhs.firstTriIndex),
        numTriangles(rhs.numTriangles),
        aabb(std::move(rhs.aabb)),
        fastFindTree(std::move(rhs.fastFindTree))
    {
    }

    /** Default move assignment operator. */
    SubMesh& SubMesh::operator=(SubMesh&& rhs)
    {
        this->~SubMesh();
        objectName = std::move(rhs.objectName);
        mtlChunks = std::move(rhs.mtlChunks);
        lineHasTexture = rhs.lineHasTexture;
        faceHasTexture = rhs.faceHasTexture;
        faceHasNormal = rhs.faceHasNormal;
        pointIndices = std::move(rhs.pointIndices);
        lineIndices = std::move(rhs.lineIndices);
        curves = std::move(rhs.curves);
        curv2es = std::move(rhs.curv2es);
        surfaces = std::move(rhs.surfaces);
        trianglePtsIndices = std::move(rhs.trianglePtsIndices);
        firstTriIndex = rhs.firstTriIndex;
        numTriangles = rhs.numTriangles;
        aabb = std::move(rhs.aabb);
        fastFindTree = std::move(rhs.fastFindTree);
        return *this;
    }

    /**
     * Reserves memory in the sub-mesh according to the simplex count
     * @param countState the state of the simplex count
     */
    void SubMesh::ReserveSubMesh(ObjCountState& countState)
    {
        pointIndices.reserve(countState.numPoints);
        lineIndices.reserve(countState.numLineIndices);
        faceIndices.reserve(countState.numFaceIndices);
        curves.reserve(countState.numCurv);
        curv2es.reserve(countState.numCurv2);
        surfaces.reserve(countState.numSurf);
        mtlChunks.reserve(countState.numMtls);
        countState.ResetSubMesh();
    }

    /**
     * Finishes a material chunk.
     * @param matChunk the chunk to finish
     */
    void SubMesh::FinishMaterial(SubMeshMaterialChunk& matChunk)
    {
        matChunk.Finish(static_cast<unsigned int>(pointIndices.size()),
            static_cast<unsigned int>(lineIndices.size()),
            static_cast<unsigned int>(faceIndices.size()));
        if (!matChunk.IsEmpty()) {
            mtlChunks.push_back(matChunk);
        }
    }
}
