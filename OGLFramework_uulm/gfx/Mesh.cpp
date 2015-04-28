/**
 * @file   Mesh.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 *
 * @brief  Contains the implementation of the Mesh class.
 */

#include "Mesh.h"

/**
 * Creates a new SubMesh in the mesh.
 * @param subMeshName the name of the sub-mesh (may be empty, then the data is saved in the mesh itself)
 * @param countState the counts of the single objects to store
 */
void Mesh::createSubMesh(const std::string& subMeshName, ObjCountState& countState)
{
    SubMesh* subMesh;
    if (subMeshName.length() == 0) {
        subMesh = this;
    } else {
        subMesh = new SubMesh(subMeshName);
        subMeshMap[subMeshName] = std::unique_ptr<SubMesh>(subMesh);
    }

    subMesh->ReserveSubMesh(countState);
}

/**
 * Reserves memory for the meshes data.
 * @param countState the counts of the single objects to store
 */
void Mesh::ReserveMesh(ObjCountState& countState)
{
    vertices.reserve(countState.numVerts);
    texCoords.reserve(countState.numTexCoords);
    normals.reserve(countState.numNormals);
    paramVertices.reserve(countState.numParameterVerts);
    lineVertices.reserve(countState.numVerts);
    faceVertices.reserve(countState.numVerts);

    subMeshes.reserve(subMeshMap.size());
    for (const auto& submesh : subMeshMap) {
        subMeshes.push_back(submesh.second.get());
    }
}
