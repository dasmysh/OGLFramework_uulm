/**
 * @file   SubMesh.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 *
 * @brief  Contains the sub-mesh class.
 */

#ifndef SUBMESH_H
#define SUBMESH_H

#include "SubMeshMaterialChunk.h"
#include "FreeFormObjects.h"

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
    /** Holds the number of line indices (per submesh). */
    unsigned int numLineIndices;
    /** Holds the number of face indices (per submesh). */
    unsigned int numFaceIndices;
    /** Holds the number of curves on a surface (per submesh). */
    unsigned int numCurv2;
    /** Holds the number of curves (per submesh). */
    unsigned int numCurv;
    /** Holds the number of surfaces (per submesh). */
    unsigned int numSurf;
    /** Holds the number of materials (per submesh). */
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
    SubMesh(const std::string& name);
    virtual ~SubMesh() {};

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
};

#endif /* SUBMESH_H */
