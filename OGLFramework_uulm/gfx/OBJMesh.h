/**
 * @file   OBJMesh.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   8. Januar 2014
 *
 * @brief  Contains the definition of OBJMesh.
 */

#ifndef OBJMESH_H
#define OBJMESH_H

#include "main.h"

#include <boost/regex.hpp>

#include "core/MaterialLibManager.h"
#include "Mesh.h"

struct ObjCountState;
struct CacheEntry;

/**
 * @brief  Resource implementation for .obj files.
 * This is used to generate renderable meshes from .obj files.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   8. Januar 2014
 */
class OBJMesh : public Resource, public Mesh
{
private:
    /** Deleted copy constructor. */
    OBJMesh(const OBJMesh& orig) : Resource(orig) {};
    /** Deleted assignment operator. */
    OBJMesh& operator=(const OBJMesh&) {};

public:
    OBJMesh(const std::string& objFilename, ApplicationBase* app);
    virtual ~OBJMesh();

    virtual void Load() override;
    virtual void Unload() override;

private:
    void UnloadLocal();

    void createMeshData(std::ifstream& file);
    void loadMeshData(std::ifstream& file);

    static void loadGroup(SubMesh* oldMesh);

    std::vector<MaterialLibrary*> getMtlLibraries(const std::string& line);
    SubMeshMaterialChunk addMtlChunkToMesh(SubMesh* mesh, SubMeshMaterialChunk& oldChunk,
            std::vector<MaterialLibrary*> matLibs, const std::string& newMtl);

    void addPointsToMesh(SubMesh* mesh, const std::string& line);
    void addLineToMesh(SubMesh* mesh, std::vector<std::unique_ptr<CacheEntry> >& cache, const std::string& line);
    void addFaceToMesh(SubMesh* mesh, std::vector<std::unique_ptr<CacheEntry> >& cache, const std::string& line);
    void addCurvToMesh(SubMesh* mesh, const std::string& line);
    void addCurv2ToMesh(SubMesh* mesh, const std::string& line);
    void addSurfToMesh(SubMesh* mesh, const std::string& line);

    static void notImplemented(const std::string & feature);
};

#endif /* OBJMESH_H */
