/**
 * @file   Mesh.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.13
 *
 * @brief  Contains the definition of the Mesh class.
 */

#ifndef MESH_H
#define MESH_H

#include "Vertices.h"
#include "SubMesh.h"

namespace cgu {

    /**
     * @brief  Base class for all meshes.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.13
     */
    class Mesh : public SubMesh
    {
    public:
        Mesh();
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&);
        Mesh& operator=(Mesh&&);
        ~Mesh();

        void createSubMesh(const std::string& subMeshName, ObjCountState& countState);
        void ReserveMesh(ObjCountState& countState);
        unsigned int FindContainingTriangle(const glm::vec3 point);
        unsigned int GetNumberOfTriangles() const;

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

        /** Holds a list of triangles with connectivity information. */
        std::vector<MeshConnectTriangle> triangleConnect;
        /** Holds a list of vertex connectivity information. */
        std::vector<MeshConnectVertex> verticesConnect;

    protected:
        void CalculateNormals();
        void CreateGeomertyInfo();
        int FindContainingTriangleSub(const SubMesh* submesh, const glm::vec3 point);
        void CreateGeomertyInfoSub(SubMesh* submesh);
        void CreateConnectivity(SubMesh* submesh);
        void CreateAABB(SubMesh* submesh);
        void CreateRTree(SubMesh* submesh);

    };
}

#endif /* MESH_H */
