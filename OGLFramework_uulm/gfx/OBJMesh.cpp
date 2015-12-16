/**
 * @file   OBJMesh.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   8. Januar 2014
 *
 * @brief  Contains the implementation of OBJMesh.
 */

#define GLM_SWIZZLE
#include "OBJMesh.h"
#include "app/ApplicationBase.h"
#include "app/Configuration.h"
#include "gfx/glrenderer/GLTexture.h"
#include "gfx/glrenderer/GLUniformBuffer.h"

#include <fstream>
#include <codecvt>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

namespace cgu {

    /** Regex string for a lines vertex index. */
    const std::string regstr_lindex = "\\s+(" + regex_help::integ
        + "(/" + regex_help::integ + ")?)";
    /** Regex string for a faces vertex index. */
    const std::string regstr_findex = "\\s+(" + regex_help::integ
        + "(/" + regex_help::integ + "?/" + regex_help::integ + "|/" + regex_help::integ + ")?)";
    /** Regex string for a vertex index list. */
    const std::string regstr_vlist = "(\\s+" + regex_help::integ + ")+";
    /** Regex string for a faces index list. */
    const std::string regstr_fidxlist = "(" + regstr_findex + ")+";

    /** Regex for parsing the <code>o</code> statement. */
    const boost::regex reg_o("^o\\s+(\\w+)$");

    /** Regex for parsing the <code>v</code> statement. */
    const boost::regex reg_v("^v\\s+(" + regex_help::flt3 + "|" + regex_help::flt4 + ")$");
    /** Regex for parsing the <code>vt</code> statement. */
    const boost::regex reg_vt("^vt\\s+(" + regex_help::flt2 + "|" + regex_help::flt3 + ")$");
    /** Regex for parsing the <code>vn</code> statement. */
    const boost::regex reg_vn("^vn\\s+" + regex_help::flt3 + "$");
    /** Regex for parsing the <code>vp</code> statement. */
    const boost::regex reg_vp("^vp\\s+(" + regex_help::flt2 + "|" + regex_help::flt3 + ")$");

    /** Regex for parsing the <code>p</code> statement. */
    const boost::regex reg_p("^p" + regstr_vlist + "$");
    /** Regex for parsing the <code>l</code> statement. */
    const boost::regex reg_l("^l(" + regstr_lindex + ")+$");
    /** Regex for parsing the <code>f</code> statement. */
    const boost::regex reg_f("^f" + regstr_fidxlist + "$");

    /** Regex for parsing the <code>curv</code> statement. */
    const boost::regex reg_curv("^curv\\s+" + regex_help::flt2 + regstr_vlist + "$");
    /** Regex for parsing the <code>curv2</code> statement. */
    const boost::regex reg_curv2("^curv2" + regstr_vlist + "$");
    /** Regex for parsing the <code>surf</code> statement. */
    const boost::regex reg_surf("^surf\\s+" + regex_help::flt4 + regstr_fidxlist + "$");

    /** Regex for parsing the <code>mtllib</code> statement. */
    const boost::regex reg_mtllib("^mtllib\\s+(\\w+\\.mtl)$");
    /** Regex for parsing the <code>usemtl</code> statement. */
    const boost::regex reg_usemtl("^usemtl\\s+(\\w+)$");

    /** Regex for counting/parsing point indices. */
    const boost::regex reg_pcount(regex_help::integ);
    /** Regex for counting/parsing line vertices. */
    const boost::regex reg_lcount(regstr_lindex);
    /** Regex for counting/parsing faces vertices. */
    const boost::regex reg_fcount(regstr_findex);

    /**
     * Used for finding existing vertices.
     * @internal
     */
    struct CacheEntry
    {
        /** Default constructor. */
        CacheEntry() : index(-1), next(nullptr) {};

        /** Holds the vertices index in der vertex buffer. */
        int index;
        /** Holds the next cache entry for vertices with this position. */
        std::unique_ptr<CacheEntry> next;
    };

    /**
     * Constructor.
     * @param objFilename the .obj files file name.
     * @param mtlLibManager the material library manager to use
     */
    OBJMesh::OBJMesh(const std::string& objFilename, ApplicationBase* app) :
        Resource(objFilename, app)
    {
    }

    OBJMesh::~OBJMesh()
    {
        if (IsLoaded()) UnloadLocal();
    }

    void OBJMesh::Load()
    {
        std::string currLine;
        std::ifstream inFile(application->GetConfig().resourceBase + "/" + id);

        if (!inFile.is_open()) {
            throw std::runtime_error("Could not open file: " + id);
        }

        createMeshData(inFile);

        inFile.clear();
        inFile.seekg(0, std::ios_base::beg);

        loadMeshData(inFile);

        while (inFile.good()) {
            std::getline(inFile, currLine);
            boost::trim(currLine);

            if (currLine.length() == 0 || boost::starts_with(currLine, "#"))
                continue; // comment or empty line
        }
        Resource::Load();
    }

    void OBJMesh::UnloadLocal()
    {

    }

    void OBJMesh::Unload()
    {
        UnloadLocal();
        Resource::Unload();
    }

    /**
     * Creates the internal structure of the mesh.
     * @param file the mesh file to read
     */
    void OBJMesh::createMeshData(std::ifstream& file)
    {
        std::string currLine;
        boost::smatch lineMatch;

        std::string currGroup = "";
        ObjCountState countState;

        while (file.good()) {
            std::getline(file, currLine);
            boost::trim(currLine);

            if (currLine.length() == 0 || boost::starts_with(currLine, "#"))
                continue; // comment or empty line
            else if (boost::regex_match(currLine, lineMatch, reg_o)) {
                createSubMesh(currGroup, countState);
                currGroup = lineMatch[1].str();
            } else if (boost::regex_match(currLine, lineMatch, reg_v)) {
                countState.numVerts++;
            } else if (boost::regex_match(currLine, lineMatch, reg_vt)) {
                countState.numTexCoords++;
            } else if (boost::regex_match(currLine, lineMatch, reg_vn)) {
                countState.numNormals++;
            } else if (boost::regex_match(currLine, lineMatch, reg_vp)) {
                countState.numParameterVerts++;
            } else if (boost::regex_match(currLine, lineMatch, reg_p)) {
                countState.numPoints += static_cast<unsigned int>(std::distance(
                    boost::sregex_iterator(currLine.begin(),
                    currLine.end(), reg_pcount), boost::sregex_iterator()));
            } else if (boost::regex_match(currLine, lineMatch, reg_l)) {
                unsigned int lineInds = static_cast<unsigned int>(std::distance(
                    boost::sregex_iterator(currLine.begin(), currLine.end(), reg_lcount),
                    boost::sregex_iterator()));
                countState.numLineIndices += (lineInds - 1) * 2;
            } else if (boost::regex_match(currLine, lineMatch, reg_f)) {
                unsigned int faceInds = static_cast<unsigned int>(std::distance(
                    boost::sregex_iterator(currLine.begin(), currLine.end(), reg_fcount),
                    boost::sregex_iterator()));
                countState.numFaceIndices += (faceInds - 2) * 3;
            } else if (boost::regex_match(currLine, lineMatch, reg_curv)) {
                countState.numCurv++;
            } else if (boost::regex_match(currLine, lineMatch, reg_curv2)) {
                countState.numCurv2++;
            } else if (boost::regex_match(currLine, lineMatch, reg_surf)) {
                countState.numSurf++;
            } else if (boost::regex_match(currLine, lineMatch, reg_usemtl)) {
                countState.numMtls++;
            }
        }
        createSubMesh(currGroup, countState);

        this->ReserveMesh(countState);
    }

    /**
     * Loads the mesh data from a file into a previously created mesh structure.
     * @param file the file to load from
     */
    void OBJMesh::loadMeshData(std::ifstream& file)
    {
        std::string currLine;
        boost::smatch lineMatch;
        SubMesh* subMesh = this;
        std::vector<MaterialLibrary*> mtlLibraries;
        SubMeshMaterialChunk mtlChunk;
        std::vector<std::unique_ptr<CacheEntry> > vfCache(vertices.capacity());
        std::vector<std::unique_ptr<CacheEntry> > vlCache(vertices.capacity());

        while (file.good()) {
            std::getline(file, currLine);
            boost::trim(currLine);

            if (currLine.length() == 0 || boost::starts_with(currLine, "#"))
                continue; // comment or empty line
            else if (boost::regex_match(currLine, lineMatch, reg_o)) {
                subMesh->FinishMaterial(mtlChunk);
                loadGroup(subMesh);

                subMesh = subMeshMap[lineMatch[1].str()].get();
                mtlChunk = SubMeshMaterialChunk(mtlChunk.material);
            } else if (boost::regex_match(currLine, lineMatch, reg_v)) {
                if (lineMatch[2].length() == 0) {
                    vertices.push_back(glm::vec4(boost::lexical_cast<float>(lineMatch[5].str()),
                        boost::lexical_cast<float>(lineMatch[6].str()),
                        boost::lexical_cast<float>(lineMatch[7].str()),
                        boost::lexical_cast<float>(lineMatch[8].str())));
                } else {
                    vertices.push_back(glm::vec4(boost::lexical_cast<float>(lineMatch[2].str()),
                        boost::lexical_cast<float>(lineMatch[3].str()),
                        boost::lexical_cast<float>(lineMatch[4].str()), 1.0f));
                }
            } else if (boost::regex_match(currLine, lineMatch, reg_vt)) {
                if (lineMatch[2].length() == 0) {
                    texCoords.push_back(glm::vec3(boost::lexical_cast<float>(lineMatch[4].str()),
                        boost::lexical_cast<float>(lineMatch[5].str()),
                        boost::lexical_cast<float>(lineMatch[6].str())));
                } else {
                    texCoords.push_back(glm::vec3(boost::lexical_cast<float>(lineMatch[2].str()),
                        boost::lexical_cast<float>(lineMatch[3].str()), 1.0f));
                }
            } else if (boost::regex_match(currLine, lineMatch, reg_vn)) {
                glm::vec3 vn(boost::lexical_cast<float>(lineMatch[1].str()),
                    boost::lexical_cast<float>(lineMatch[2].str()),
                    boost::lexical_cast<float>(lineMatch[3].str()));
                normals.push_back(glm::normalize(vn));
            } else if (boost::regex_match(currLine, lineMatch, reg_vp)) {
                if (lineMatch[2].length() == 0) {
                    paramVertices.push_back(glm::vec3(boost::lexical_cast<float>(lineMatch[4].str()),
                        boost::lexical_cast<float>(lineMatch[5].str()),
                        boost::lexical_cast<float>(lineMatch[6].str())));
                } else {
                    paramVertices.push_back(glm::vec3(boost::lexical_cast<float>(lineMatch[2].str()),
                        boost::lexical_cast<float>(lineMatch[3].str()), 1.0f));
                }
            } else if (boost::regex_match(currLine, lineMatch, reg_p)) {
                OBJMesh::addPointsToMesh(subMesh, currLine);
            } else if (boost::regex_match(currLine, lineMatch, reg_l)) {
                OBJMesh::addLineToMesh(subMesh, vlCache, currLine);
            } else if (boost::regex_match(currLine, lineMatch, reg_f)) {
                OBJMesh::addFaceToMesh(subMesh, vfCache, currLine);
            } else if (boost::regex_match(currLine, lineMatch, reg_curv)) {
                OBJMesh::addCurvToMesh(subMesh, currLine);
            } else if (boost::regex_match(currLine, lineMatch, reg_curv2)) {
                OBJMesh::addCurv2ToMesh(subMesh, currLine);
            } else if (boost::regex_match(currLine, lineMatch, reg_surf)) {
                OBJMesh::addSurfToMesh(subMesh, currLine);
            } else if (boost::regex_match(currLine, lineMatch, reg_mtllib)) {
                mtlLibraries = getMtlLibraries(currLine);
            } else if (boost::regex_match(currLine, lineMatch, reg_usemtl)) {
                mtlChunk = addMtlChunkToMesh(subMesh, mtlChunk, mtlLibraries, lineMatch[1].str());
            }
        }

        subMesh->FinishMaterial(mtlChunk);
        loadGroup(subMesh);
        CreateGeomertyInfo();
        if (!this->faceHasNormal) CalculateNormals();
    }

    std::vector<MaterialLibrary*> OBJMesh::getMtlLibraries(const std::string& line)
    {
        boost::regex reg_mtllibname("\\s+(\\w+\\.mtl)");
        boost::sregex_iterator i(line.begin(), line.end(), reg_mtllibname);
        boost::sregex_iterator j;
        std::vector<MaterialLibrary*> result;
        while (i != j) {
            boost::filesystem::path meshFile{ id };
            std::string mtllibname = meshFile.parent_path().string() + "/" + (*i++)[1].str();
            result.push_back(application->GetMaterialLibManager()->GetResource(mtllibname));
        }
        return result;
    }

    void OBJMesh::loadGroup(SubMesh* submesh)
    {
    }

    SubMeshMaterialChunk OBJMesh::addMtlChunkToMesh(SubMesh* mesh, SubMeshMaterialChunk& oldChunk,
        std::vector<MaterialLibrary*> matLibs, const std::string& newMtl)
    {
        mesh->FinishMaterial(oldChunk);

        // get new material
        const Material* newMat = nullptr;
        for (MaterialLibrary* lib : matLibs) {
            if (lib->HasResource(newMtl)) {
                newMat = lib->GetResource(newMtl);
            }
        }
        return SubMeshMaterialChunk(oldChunk, newMat);
    }

    void OBJMesh::addPointsToMesh(SubMesh* mesh, const std::string& line)
    {
        boost::sregex_token_iterator i(line.begin(), line.end(), reg_pcount);
        boost::sregex_token_iterator j;
        while (i != j) {
            int pi = boost::lexical_cast<int>(*i++);
            mesh->pointIndices.push_back(pi > 0 ? static_cast<GLuint> (pi - 1)
                : static_cast<GLuint>(vertices.size() - pi));
        }
    }

    /**
     * Adds a new vertex to an array of vertices checking if it does already exist.
     * @tparam VT the vertex type
     * @param[in,out] vertices the vertex array to add the vertex to
     * @param[in] v the vertex to add
     * @param[in] pidx the index of the vertices position (used for faster finding existing vertices)
     * @param[in,out] cache a caching array used for faster finding existing vertices
     * @return the index (in vertices) of the vertex to add
     */
    template<class VT>
    unsigned int addVertex(std::vector<VT>& vertices, const VT& v, unsigned int pidx,
        std::vector<std::unique_ptr<CacheEntry> >& cache)
    {
        CacheEntry* ce = cache[pidx].get();

        while (ce != nullptr) {
            unsigned int idx = ce->index;
            if (vertices[idx] == v)
                return idx;
            ce = ce->next.get();
        }

        unsigned int idx = static_cast<unsigned int>(vertices.size());
        vertices.push_back(v);
        ce = new CacheEntry;
        ce->next = nullptr;
        ce->index = idx;
        if (cache[pidx] != nullptr) {
            CacheEntry* cei = cache[pidx].get();
            while (cei->next != nullptr) {
                cei = cei->next.get();
            }
            cei->next = std::unique_ptr<CacheEntry>(ce);
        } else {
            cache[pidx] = std::unique_ptr<CacheEntry>(ce);
        }
        return idx;
    }

    void OBJMesh::addLineToMesh(SubMesh* mesh, std::vector<std::unique_ptr<CacheEntry> >& cache, const std::string& line)
    {
        boost::sregex_iterator i(line.begin(), line.end(), reg_lcount);
        boost::sregex_iterator j;
        bool first = true;
        while (i != j) {
            LineVertex lv;
            int pi = boost::lexical_cast<int>((*i)[2].str());
            pi = pi > 0 ? static_cast<unsigned int> (pi - 1) : static_cast<int>(vertices.size() - pi);
            lv.pos = vertices[pi].xyz();
            if ((*i)[4].length() > 0) {
                mesh->lineHasTexture = true;
                int ti = boost::lexical_cast<int>((*i)[4].str());
                lv.tex = texCoords[ti > 0 ? static_cast<unsigned int> (ti - 1)
                    : static_cast<int>(texCoords.size() - ti)].xy();
            }
            i++;

            unsigned int idx = addVertex(lineVertices, lv, pi, cache);
            mesh->lineIndices.push_back(idx);
            boost::sregex_iterator ti = i;
            ti++;
            if (first || ti == j) {
                first = false;
            } else {
                mesh->lineIndices.push_back(idx);
            }
        }
    }

    void OBJMesh::addFaceToMesh(SubMesh* mesh, std::vector<std::unique_ptr<CacheEntry> >& cache, const std::string& line)
    {
        boost::sregex_iterator i(line.begin(), line.end(), reg_fcount);
        boost::sregex_iterator j;
        int idx1 = -1;
        int idx2 = -1;
        int pidx1 = -1;
        int pidx2 = -1;
        std::set<MeshConnectTriangle>::iterator currentFace = mesh->trianglePtsIndices.end();
        while (i != j) {
            FaceVertex fv;
            int pi = boost::lexical_cast<int>((*i)[2].str());
            pi = pi > 0 ? static_cast<unsigned int> (pi - 1) : static_cast<int>(vertices.size() - pi);
            fv.pos = vertices[pi].xyz();
            if (pidx1 == -1)
                pidx1 = pi;
            else if (pidx2 == -1)
                pidx2 = pi;
            else {
                auto insResult = mesh->trianglePtsIndices.insert(MeshConnectTriangle({ { pidx1, pidx2, pi } }));
                assert(insResult.second || "Check why this happened...");
                currentFace = insResult.first;
                pidx2 = pi;
            }

            if ((*i)[4].length() > 0) {
                mesh->faceHasTexture = true;
                mesh->faceHasNormal = true;
                int ti = boost::lexical_cast<int>((*i)[4].str());
                int ni = boost::lexical_cast<int>((*i)[5].str());
                fv.tex = texCoords[ti > 0 ? static_cast<unsigned int> (ti - 1) : texCoords.size() - ti].xy();
                fv.normal = normals[ni > 0 ? static_cast<unsigned int> (ni - 1) : normals.size() - ni];
            } else if ((*i)[6].length() > 0) {
                mesh->faceHasTexture = true;
                mesh->faceHasNormal = false;
                int ti = boost::lexical_cast<int>((*i)[6].str());
                fv.tex = texCoords[ti > 0 ? static_cast<unsigned int> (ti - 1) : texCoords.size() - ti].xy();
                fv.normal = glm::vec3(0.0f);
            } else if ((*i)[5].length() > 0) {
                mesh->faceHasNormal = true;
                mesh->faceHasTexture = false;
                int ni = boost::lexical_cast<int>((*i)[5].str());
                fv.tex = glm::vec2(0.0f);
                fv.normal = normals[ni > 0 ? static_cast<unsigned int> (ni - 1) : normals.size() - ni];
            }
            i++;

            unsigned int idx = addVertex(faceVertices, fv, pi, cache);
            if (idx1 == -1)
                idx1 = idx;
            else if (idx2 == -1)
                idx2 = idx;
            else {
                mesh->faceIndices.push_back(idx1);
                mesh->faceIndices.push_back(idx2);
                mesh->faceIndices.push_back(idx);
                currentFace->faceVertex[0] = idx1;
                currentFace->faceVertex[1] = idx2;
                currentFace->faceVertex[2] = idx;

                idx2 = idx;
            }
        }
    }

    void OBJMesh::addCurvToMesh(SubMesh*, const std::string & line)
    {
        notImplemented(line);
    }

    void OBJMesh::addCurv2ToMesh(SubMesh*, const std::string & line)
    {
        notImplemented(line);
    }

    void OBJMesh::addSurfToMesh(SubMesh*, const std::string & line)
    {
        notImplemented(line);
    }

    /**
     * Logs a warning this feature is not implemented.
     * @param feature the line with the feature to log
     */
    void OBJMesh::notImplemented(const std::string & feature)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        LOG(WARNING) << L"Feature not implemented: " << converter.from_bytes(feature);
    }
}
