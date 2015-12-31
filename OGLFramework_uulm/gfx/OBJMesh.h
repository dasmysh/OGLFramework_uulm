/**
 * @file   OBJMesh.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.08
 *
 * @brief  Contains the definition of OBJMesh.
 */

#ifndef OBJMESH_H
#define OBJMESH_H

#include "main.h"
#include "core/MaterialLibManager.h"
#include "Mesh.h"

namespace cgu {

    struct ObjCountState;
    struct CacheEntry;

    /**
     * @brief  Resource implementation for .obj files.
     * This is used to generate renderable meshes from .obj files.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.08
     */
    class OBJMesh : public Resource, public Mesh
    {
    public:
        OBJMesh(const std::string& objFilename, ApplicationBase* app);
        OBJMesh(const OBJMesh&);
        OBJMesh& operator=(const OBJMesh&);
        OBJMesh(OBJMesh&&);
        OBJMesh& operator=(OBJMesh&&);
        virtual ~OBJMesh();

        void Load() override;
        void Unload() override;

    private:
        void createMeshData(std::ifstream& file);
        void loadMeshData(std::ifstream& file);

        static void loadGroup(SubMesh* oldMesh);

        std::vector<MaterialLibrary*> getMtlLibraries(const std::string& line) const;
        static SubMeshMaterialChunk addMtlChunkToMesh(SubMesh* mesh, SubMeshMaterialChunk& oldChunk,
            std::vector<MaterialLibrary*> matLibs, const std::string& newMtl);

        void addPointsToMesh(SubMesh* mesh, const std::string& line) const;
        void addLineToMesh(SubMesh* mesh, std::vector<std::unique_ptr<CacheEntry> >& cache, const std::string& line);
        void addFaceToMesh(SubMesh* mesh, std::vector<std::unique_ptr<CacheEntry> >& cache, const std::string& line);
        static void addCurvToMesh(SubMesh* mesh, const std::string& line);
        static void addCurv2ToMesh(SubMesh* mesh, const std::string& line);
        static void addSurfToMesh(SubMesh* mesh, const std::string& line);

        static void notImplemented(const std::string & feature);
    };
}

#endif /* OBJMESH_H */
