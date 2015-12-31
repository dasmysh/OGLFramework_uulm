/**
 * @file   VolumeBrickOctree.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.17
 *
 * @brief  Implementation of the layered octree.
 */

#include "VolumeBrickOctree.h"
#include "gfx/glrenderer/GLTexture3D.h"
#include "gfx/glrenderer/GLTexture.h"
#include "app/ApplicationBase.h"
#include <boost/assign.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cgu {

    namespace cguOctreeMath {
        inline unsigned int calculateOverlapPixels(unsigned int val) {
            return (val / VolumeBrickOctree::MAX_SIZE) << 1;
        }
    }

    VolumeBrickOctree::VolumeBrickOctree(const glm::uvec3& pos, const glm::uvec3& size, const glm::vec3& scale,
        unsigned int lvl, GPUProgram* minMaxProg, const std::vector<BindingLocation> uniformNames) :
        posOffset(pos),
        origSize(size),
        voxelScale(scale),
        level(lvl),
        hasAnyData(true),
        minTexValue(0.0f),
        maxTexValue(1.0f),
        maxLevel(0),
        streamFile(nullptr),
        dataSize(0),
        brickTextureDesc(4, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE),
        minMaxMapProgram(minMaxProg),
        minMaxMapUniformNames(uniformNames)
    {
    }

    /**
     *  Constructor.
     *  @param texData the 3D texture resource to create the tree from.
     *  @param pos the position in the texture.
     *  @param size the size of this part of the tree (in voxels).
     *  @param scale the scale of a voxel in this tree.
     *  @param minMaxProg the program used to create Min-/Max-Maps.
     *  @param uniformNames the uniform names of the minMaxProg.
     *  @param app the application object.
     */
    VolumeBrickOctree::VolumeBrickOctree(const GLTexture3D* texData, const glm::uvec3& pos, const glm::uvec3& size,
        const glm::vec3& scale, GPUProgram* minMaxProg, const std::vector<BindingLocation> uniformNames,
        ApplicationBase* app) :
        VolumeBrickOctree(pos, size, scale, 0, minMaxProg, uniformNames)
    {
        if (origSize.x > MAX_SIZE || origSize.y > MAX_SIZE || origSize.z > MAX_SIZE) {
            auto ovlp = cguOctreeMath::calculateOverlapPixels(glm::max(origSize.x, glm::max(origSize.y, origSize.z)));
            glm::uvec3 sizeOverlap{ ovlp };
            auto sizeWithOverlap = origSize + sizeOverlap;
            glm::uvec3 sizePowerOfTwo{ cguMath::roundupPow2(origSize.x), cguMath::roundupPow2(origSize.y),
                cguMath::roundupPow2(origSize.z) };
            if (sizeWithOverlap.x > sizePowerOfTwo.x) {
                sizeWithOverlap.x = cguMath::roundupPow2(sizeWithOverlap.x);
                sizeOverlap <<= 1;
            } else sizeWithOverlap.x = sizePowerOfTwo.x;
            if (sizeWithOverlap.y > sizePowerOfTwo.y) {
                sizeWithOverlap.y = cguMath::roundupPow2(sizeWithOverlap.y);
                sizeOverlap <<= 1;
            } else sizeWithOverlap.y = sizePowerOfTwo.y;
            if (sizeWithOverlap.z > sizePowerOfTwo.z) {
                sizeWithOverlap.z = cguMath::roundupPow2(sizeWithOverlap.z);
                sizeOverlap <<= 1;
            } else sizeWithOverlap.z = sizePowerOfTwo.z;
            glm::uvec3 childSizeBase{ sizeWithOverlap.x >> 1, sizeWithOverlap.y >> 1, sizeWithOverlap.z >> 1 };

            CreateNode(childSizeBase, texData, app);
        } else {
            CreateLeafTexture(texData);
        }
    }

    VolumeBrickOctree::VolumeBrickOctree(const GLTexture3D* texData, const glm::uvec3& pos,
        const glm::uvec3& size, unsigned int lvl, const glm::vec3& scale, GPUProgram* minMaxProg,
        const std::vector<BindingLocation> uniformNames, ApplicationBase* app) :
        VolumeBrickOctree(pos, size, scale, lvl, minMaxProg, uniformNames)
    {
        if (origSize.x > MAX_SIZE || origSize.y > MAX_SIZE || origSize.z > MAX_SIZE) {
            glm::uvec3 sizePowerOfTwo{ cguMath::roundupPow2(origSize.x), cguMath::roundupPow2(origSize.y),
                cguMath::roundupPow2(origSize.z) };
            glm::uvec3 childSizeBase{ sizePowerOfTwo.x >> 1, sizePowerOfTwo.y >> 1, sizePowerOfTwo.z >> 1 };

            CreateNode(childSizeBase, texData, app);

        } else {
            CreateLeafTexture(texData);
        }
    }
    
    void VolumeBrickOctree::CreateNode(const glm::uvec3& childSizeBase, const GLTexture3D* texData, ApplicationBase* app)
    {
        glm::uvec3 posOffsets[8];
        posOffsets[0] = glm::uvec3(0, 0, 0);
        posOffsets[1] = glm::uvec3(0, 0, 1);
        posOffsets[2] = glm::uvec3(0, 1, 0);
        posOffsets[3] = glm::uvec3(0, 1, 1);
        posOffsets[4] = glm::uvec3(1, 0, 0);
        posOffsets[5] = glm::uvec3(1, 0, 1);
        posOffsets[6] = glm::uvec3(1, 1, 0);
        posOffsets[7] = glm::uvec3(1, 1, 1);

        auto ovlp = cguOctreeMath::calculateOverlapPixels(glm::max(childSizeBase.x, glm::max(childSizeBase.y, childSizeBase.z)));

        for (unsigned int i = 0; i < 8; ++i) {
            auto childPosOffset = posOffsets[i] * (childSizeBase - glm::uvec3(ovlp));
            auto childPos = posOffset + childPosOffset;
            auto childSize = glm::uvec3(glm::max(glm::ivec3(0),
                glm::ivec3(glm::min(origSize, childPosOffset + childSizeBase)) - glm::ivec3(childPosOffset)));

            if (childSizeBase.x == ovlp && posOffsets[i].x == 1) childSize.x = 0;
            if (childSizeBase.y == ovlp && posOffsets[i].y == 1) childSize.y = 0;
            if (childSizeBase.z == ovlp && posOffsets[i].z == 1) childSize.z = 0;

            children[i].reset(new VolumeBrickOctree(texData, childPos, childSize, level + 1, voxelScale,
                minMaxMapProgram, minMaxMapUniformNames, app));
        }
        texSize.x = (children[0]->texSize.x + children[4]->texSize.x) >> 1;
        texSize.y = (children[0]->texSize.y + children[2]->texSize.y) >> 1;
        texSize.z = (children[0]->texSize.z + children[1]->texSize.z) >> 1; 

        for (auto& child : children) {
            maxLevel = glm::max(maxLevel, child->maxLevel);
        }

        CalculateTexBorders(texData);

        CombineChildTextures(app);
        WriteBrickTextureToTempFile();

        for (auto& child : children) child->ResetAllData();
    }

    void VolumeBrickOctree::CreateLeafTexture(const GLTexture3D* texData)
    {
        texSize = origSize;
        maxLevel = level;

        if (texSize.x * texSize.y * texSize.z == 0) {
            dataSize = 0;
            hasAnyData = false;
            return;
        }

        CalculateTexBorders(texData);

        brickTexture = texData->CreateMinMaxTexture(posOffset, origSize, brickTextureDesc);
        WriteBrickTextureToTempFile();
    }

    void VolumeBrickOctree::CalculateTexBorders(const GLTexture3D* texData)
    {
        if (posOffset.x != 0) minTexValue.x = 1.0f / static_cast<float>(texSize.x);
        if (posOffset.y != 0) minTexValue.y = 1.0f / static_cast<float>(texSize.y);
        if (posOffset.z != 0) minTexValue.z = 1.0f / static_cast<float>(texSize.z);

        if (posOffset.x + origSize.x < texData->GetSize().x) maxTexValue.x = (static_cast<float>(texSize.x) - 1.0f) / static_cast<float>(texSize.x);
        if (posOffset.y + origSize.y < texData->GetSize().y) maxTexValue.y = (static_cast<float>(texSize.y) - 1.0f) / static_cast<float>(texSize.y);
        if (posOffset.z + origSize.z < texData->GetSize().z) maxTexValue.z = (static_cast<float>(texSize.z) - 1.0f) / static_cast<float>(texSize.z);
    }

    /**
     *  Destructor.
     */
    VolumeBrickOctree::~VolumeBrickOctree()
    {
        if (streamFile) std::fclose(streamFile);
        streamFile = nullptr;
    }

    /**
     *  Reloads all data from this tree node.
     */
    void VolumeBrickOctree::ReloadData()
    {
        if (dataSize == 0) return;
        std::fseek(streamFile, 0, SEEK_SET);
        std::vector<uint8_t> data;
        data.resize(dataSize);
        std::fread(data.data(), sizeof(uint8_t), dataSize, streamFile);

        brickTexture.reset(new GLTexture(texSize.x, texSize.y, texSize.z, brickTextureDesc, nullptr));
        brickTexture->UploadData(data);
        brickTexture->GenerateMinMaxMaps(minMaxMapProgram, minMaxMapUniformNames);
        brickTexture->SampleLinear();
        brickTexture->SampleWrapClamp();
    }

    /**
     *  Releases all the data from this tree node.
     */
    void VolumeBrickOctree::ResetData()
    {
        brickTexture.reset(nullptr);
    }

    /**
     *  Releases all the data from this tree node and its children.
     */
    void VolumeBrickOctree::ResetAllData()
    {
        if (hasAnyData) {
            brickTexture.reset(nullptr);
            if (children[0]) for (auto& child : children) child->ResetAllData();
            hasAnyData = false;
        }
    }

    /**
     *  Writes the brick texture to a temporary file.
     */
    void VolumeBrickOctree::WriteBrickTextureToTempFile()
    {
        if (texSize.x * texSize.y * texSize.z == 0) {
            dataSize = 0;
            hasAnyData = false;
            return;
        }
        std::vector<uint8_t> data;
        brickTexture->DownloadData(data);

        dataSize = static_cast<unsigned int>(data.size());
        streamFile = std::tmpfile();
        std::fwrite(data.data(), sizeof(uint8_t), dataSize, streamFile);
        ReloadData();
    }

    /**
     *  Combines the 8 textures of this nodes children to a new texture.
     *  @param app the application object.
     */
    void VolumeBrickOctree::CombineChildTextures(ApplicationBase* app)
    {
        if (texSize.x * texSize.y * texSize.z == 0) return;
        GPUProgram* combineProg = nullptr;
        brickTextureDesc.bytesPP = children[0]->brickTextureDesc.bytesPP;
        brickTextureDesc.format = children[0]->brickTextureDesc.format;
        brickTextureDesc.type = children[0]->brickTextureDesc.type;
        brickTextureDesc.internalFormat = children[0]->brickTextureDesc.internalFormat;

        if (brickTextureDesc.type == GL_UNSIGNED_BYTE) combineProg = app->GetGPUProgramManager()->GetResource("combineChildTextures8.cp");
        else if (brickTextureDesc.type == GL_UNSIGNED_SHORT) combineProg = app->GetGPUProgramManager()->GetResource("combineChildTextures16.cp");
        else if (brickTextureDesc.type == GL_UNSIGNED_INT) combineProg = app->GetGPUProgramManager()->GetResource("combineChildTextures32.cp");
        else throw std::runtime_error("Pixel-type not supported.");

        auto uniformNames = combineProg->GetUniformLocations({ "childTex", "combineTex", "childShift", "maxChunkSize" });
        brickTexture.reset(new GLTexture(texSize.x, texSize.y, texSize.z, brickTextureDesc, nullptr));

        //glm::uvec3 chunkSize = glm::uvec3(glm::ceil(glm::vec3(texSize) / 2.0f));
        combineProg->UseProgram();
        combineProg->SetUniform(uniformNames[0], 0);
        combineProg->SetUniform(uniformNames[1], 1);

        glm::uvec3 childShifts[8];
        childShifts[0] = glm::uvec3(0, 0, 0);
        childShifts[1] = glm::uvec3(0, 0, 1);
        childShifts[2] = glm::uvec3(0, 1, 0);
        childShifts[3] = glm::uvec3(0, 1, 1);
        childShifts[4] = glm::uvec3(1, 0, 0);
        childShifts[5] = glm::uvec3(1, 0, 1);
        childShifts[6] = glm::uvec3(1, 1, 0);
        childShifts[7] = glm::uvec3(1, 1, 1);

        brickTexture->ActivateImage(1, 0, GL_WRITE_ONLY);
        for (unsigned int i = 0; i < 8; ++i) {
            if (children[i]->dataSize != 0) {
                auto numGroups = glm::ivec3(glm::ceil(glm::vec3(children[i]->texSize) / 8.0f));

                combineProg->SetUniform(uniformNames[3], children[i]->texSize);
                combineProg->SetUniform(uniformNames[2], childShifts[i]);
                children[i]->brickTexture->ActivateImage(0, 0, GL_READ_ONLY);
                OGL_CALL(glDispatchCompute, numGroups.x, numGroups.y, numGroups.z);
            }
        }

        OGL_CALL(glMemoryBarrier, GL_ALL_BARRIER_BITS);
        OGL_SCALL(glFinish);
    }

    /**
     *  Updates the loading state of the tree depending on the view frustum (in object space).
     *  @param camera the current camera to calculate the frustum for.
     *  @param world the world matrix of the parent node.
     *  @return whether there are any children with loaded data in the sub-tree.
     */
    bool VolumeBrickOctree::UpdateFrustum(const cgu::CameraView& camera, const glm::mat4& world)
    {
        if (dataSize == 0) return false;
        auto isCorrectLod = false; // TODO: add LOD mechanism here. [8/26/2015 Sebastian Maisch]
        // if (level == maxLevel - 1) isCorrectLod = true;

        auto localWorld = GetLocalWorld(world);

        cguMath::AABB3<float> box{ { { glm::vec3(0.0f), glm::vec3(1.0f) } } };

        if (!cguMath::AABBInFrustumTest(camera.GetViewFrustum(localWorld), box)) ResetAllData();
        else if (!children[0]) {
            if (!IsLoaded()) { ReloadData(); hasAnyData = true; }
        } else {
            if (isCorrectLod) {
                if (IsLoaded()) for (auto& child : children) child->ResetAllData();
                else { ReloadData(); hasAnyData = true; }
            } else {
                if (IsLoaded()) {
                    ResetData();
                }
                hasAnyData = false;
                for (auto& child : children) hasAnyData = child->UpdateFrustum(camera, world) || hasAnyData;
            }
        }
        return hasAnyData;
    }

    void VolumeBrickOctree::GetRenderedBricksList(const cgu::CameraView& camera, const glm::mat4& world,
        std::vector<std::pair<const VolumeBrickOctree*, float>>& result) const
    {
        if (!hasAnyData || dataSize == 0) return;
        else if (IsLoaded()) {
            auto childWorld = GetLocalWorld(world);
            result.push_back(std::make_pair(this, camera.GetSignedDistanceToUnitAABB2(childWorld)));
        }
        else {
            for (auto& child : children) child->GetRenderedBricksList(camera, world, result);
        }
    }

    glm::mat4 VolumeBrickOctree::GetLocalWorld(const glm::mat4& world) const
    {
        // glm::mat4 scaleVoxelMat = glm::scale(glm::mat4(), glm::vec3(origSize));
        auto scaleVoxelMat = glm::scale(glm::mat4(), glm::vec3(origSize) * (maxTexValue - minTexValue));
        auto scaleToWorldMat = glm::scale(glm::mat4(), voxelScale);
        // glm::mat4 translateOffsetMat = glm::translate(glm::mat4(), glm::vec3(posOffset));
        auto translateOffsetMat = glm::translate(glm::mat4(), glm::vec3(posOffset) + (minTexValue * glm::vec3(texSize)));
        return world * scaleToWorldMat * translateOffsetMat * scaleVoxelMat;
    }
}
