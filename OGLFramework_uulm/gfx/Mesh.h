/**
 * @file   Mesh.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 *
 * @brief  Contains the definition of the Mesh class.
 */

#ifndef MESH_H
#define MESH_H

#include "Vertices.h"
#include "SubMesh.h"

/**
 * @brief  Base class for all meshes.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 */
class Mesh : public SubMesh
{
public:
    void createSubMesh(const std::string& subMeshName, ObjCountState& countState);
    void ReserveMesh(ObjCountState& countState);

    /** Holds all the single points used by the mesh (and its sub-meshes) as points or in vertices. */
    std::vector<glm::vec4> vertices;
    /** Holds all the single texture coordinates used by the mesh (and its sub-meshes). */
    std::vector<glm::vec3> texCoords;
    /** Holds all the single normals used by the mesh (and its sub-meshes). */
    std::vector<glm::vec3> normals;
    /** Holds all the parameter vertices used by the mesh (and its sub-meshes). */
    std::vector<glm::vec3> paramVertices;
    /** Holds all (line-)vertices the mesh (and its sub-meshes) uses. */
    std::vector<LineVertex> lineVertices;
    /** Holds all (face-)vertices the mesh (and its sub-meshes) uses. */
    std::vector<FaceVertex> faceVertices;

    /** Holds all the meshes sub-meshes (as a map for easier loading). */
    std::unordered_map<std::string, std::unique_ptr<SubMesh> > subMeshMap;
    /** Holds all the meshes sub-meshes (as an array for fast iteration during rendering). */
    std::vector<SubMesh*> subMeshes;

};

#endif /* MESH_H */
