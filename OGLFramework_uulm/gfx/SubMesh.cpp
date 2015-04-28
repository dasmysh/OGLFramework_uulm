/**
 * @file   SubMesh.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 *
 * @brief  Implementation of the SubMesh.
 */

#include "SubMesh.h"

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

SubMesh::SubMesh() :
objectName(""),
lineHasTexture(false),
faceHasTexture(false),
faceHasNormal(false)
{
}

/**
 * Constructor.
 * @param name the submeshes name
 */
SubMesh::SubMesh(const std::string& name) : objectName(name)
{
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
