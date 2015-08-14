/**
 * @file   Mesh.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   13. Januar 2014
 *
 * @brief  Contains the implementation of the Mesh class.
 */

#define GLM_SWIZZLE

#include "Mesh.h"
#include "core/math/math.h"

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/index/rtree.hpp>

#undef min
#undef max


namespace cgu {

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

    /**
     *  Find index of triangle that contains the given point.
     *  @param point the point to find the triangle for.
     */
    unsigned int Mesh::FindContainingTriangle(const glm::vec3 point)
    {
        int result = FindContainingTriangleSub(this, point);
        if (result != -1) return result;

        for (auto submesh : subMeshes) {
            result = FindContainingTriangleSub(submesh, point);
            if (result != -1) return result;
        }
        throw std::out_of_range("Containing triangle not found!");
    }

    /**
     *  Get the number of triangles in this mesh.
     *  @return the number of triangles.
     */
    unsigned int Mesh::GetNumberOfTriangles() const
    {
        unsigned int result = numTriangles;
        for (const auto submesh : subMeshes) {
            result += submesh->numTriangles;
        }
        return result;
    }

    /**
     *  Creates the meshes geometry information and acceleration structures.
     */
    void Mesh::CreateGeomertyInfo()
    {
        verticesConnect.resize(vertices.size());
        unsigned int cid = 0;
        for (auto& vc : verticesConnect) {
            vc.idx = cid++;
        }

        CreateGeomertyInfoSub(this);

        for (auto submesh : subMeshes) {
            CreateGeomertyInfoSub(submesh);
        }
    }

    /**
     *  Find index of triangle that contains the given point.
     *  @param submesh the sub mesh to find the triangle in.
     *  @param point the point to find the triangle for.
     */
    int Mesh::FindContainingTriangleSub(const SubMesh* submesh, const glm::vec3 pt)
    {
        if (pt.x < submesh->aabb[0].x || pt.y < submesh->aabb[0].y || pt.z < submesh->aabb[0].z
            || pt.x > submesh->aabb[1].x || pt.y > submesh->aabb[1].y || pt.z > submesh->aabb[1].z)
            return -1;

        std::vector<polyIdxBox> hits;
        namespace bg = boost::geometry;
        submesh->fastFindTree.query(bg::index::contains(point(pt.x, pt.y, pt.z)), std::back_inserter(hits));
        for (const auto& polyBox : hits) {
            unsigned int triId = polyBox.second;
            cguMath::Tri3<float> tri{ { vertices[triangleConnect[triId].vertex[0]].xyz(),
                vertices[triangleConnect[triId].vertex[1]].xyz(), vertices[triangleConnect[triId].vertex[2]].xyz() } };
            if (cguMath::pointInTriangleTest<float>(tri, pt, nullptr)) return triId;
        }
        return -1;
    }

    /**
     *  Creates the meshes geometry information and acceleration structures.
     *  @param submesh the sub mesh to create geometry information for.
     */
    void Mesh::CreateGeomertyInfoSub(SubMesh* submesh)
    {
        CreateAABB(submesh);
        CreateConnectivity(submesh);
        CreateRTree(submesh);
    }

    /**
     *  Creates the sub meshes connectivity.
     *  @param submesh the sub mesh to create the connectivity for.
     */
    void Mesh::CreateConnectivity(SubMesh* submesh)
    {
        submesh->firstTriIndex = static_cast<unsigned int>(triangleConnect.size());
        submesh->numTriangles = static_cast<unsigned int>(submesh->trianglePtsIndices.size());
        triangleConnect.insert(triangleConnect.end(), submesh->trianglePtsIndices.begin(), submesh->trianglePtsIndices.end());

        // set vertex connectivity
        for (unsigned int i = submesh->firstTriIndex; i < submesh->firstTriIndex + submesh->numTriangles; ++i) {
            MeshConnectTriangle& tri = triangleConnect[i];
            verticesConnect[tri.vertex[0]].triangles.push_back(i);
            verticesConnect[tri.vertex[1]].triangles.push_back(i);
            verticesConnect[tri.vertex[2]].triangles.push_back(i);
        }

        // set triangle neighbours
        for (unsigned int i = submesh->firstTriIndex; i < submesh->firstTriIndex + submesh->numTriangles; ++i) {
            MeshConnectTriangle& tri = triangleConnect[i];
            for (unsigned int ni = 0; ni < 3; ++ni) {
                unsigned int vi0 = tri.vertex[(ni + 1) % 3];
                unsigned int vi1 = tri.vertex[(ni + 2) % 3];
                std::vector<unsigned int> isect;
                std::set_intersection(verticesConnect[vi0].triangles.begin(), verticesConnect[vi0].triangles.end(),
                    verticesConnect[vi1].triangles.begin(), verticesConnect[vi1].triangles.end(), std::back_inserter(isect));
                // if the mesh is planar and has borders only ONE triangle may be found!!!
                // this triangle is the triangle itself not its neighbour
                assert(isect.size() <= 2); // both this triangle and the neighbour should be found
                if (isect.size() == 2) {
                    tri.neighbours[ni] = isect[0] == i ? isect[1] : isect[0];
                } else {
                    tri.neighbours[ni] = -1;
                }
            }
        }
    }

    /**
     *  Creates the sub meshes bounding box.
     *  @param submesh the sub mesh to create the bounding box for.
     */
    void Mesh::CreateAABB(SubMesh* submesh)
    {
        if (trianglePtsIndices.empty()) return;
        submesh->aabb[0] = submesh->aabb[1] = vertices[trianglePtsIndices.begin()->vertex[0]].xyz();
        for (const auto& tri : trianglePtsIndices) {
            for (auto vi : tri.vertex) {
                submesh->aabb[0] = glm::min(submesh->aabb[0], vertices[vi].xyz());
                submesh->aabb[1] = glm::max(submesh->aabb[1], vertices[vi].xyz());
            }
        }
    }

    /**
     *  Creates the sub meshes r-tree for faster point in triangle tests.
     *  @param submesh the sub mesh to create the tree for.
     */
    void Mesh::CreateRTree(SubMesh* submesh)
    {
        for (unsigned int i = submesh->firstTriIndex; i < submesh->firstTriIndex + submesh->numTriangles; ++i) {
            polygon poly;
            for (unsigned int vi = 0; vi < 3; ++vi) {
                glm::vec3 vpt = vertices[triangleConnect[i].vertex[vi]].xyz();
                poly.outer().push_back(point(vpt.x, vpt.y, vpt.z));
            }
            box b = boost::geometry::return_envelope<box>(poly);
            submesh->fastFindTree.insert(std::make_pair(b, i));
        }
    }
}