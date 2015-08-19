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

namespace cgu {

    VolumeBrickOctree::VolumeBrickOctree(const GLTexture3D* texData, const glm::uvec3& pos, const glm::uvec3& size,
        GPUProgram* minMaxProg, const std::vector<BindingLocation> uniformNames, ApplicationBase* app) :
        posOffset(pos),
        origSize(size),
        streamFile(nullptr),
        minMaxMapProgram(minMaxProg),
        minMaxMapUniformNames(uniformNames)
    {
        if (size.x > MAX_SIZE || size.y > MAX_SIZE || size.z > MAX_SIZE) {
            // size + 1 to have one pixel overlap
            glm::uvec3 overlapAdj(size.x > MAX_SIZE ? 1 : 0, size.y > MAX_SIZE ? 1 : 0, size.z > MAX_SIZE ? 1 : 0);
            glm::vec3 fSize(static_cast<float>(size.x + overlapAdj.x), static_cast<float>(size.y + overlapAdj.y), static_cast<float>(size.z + overlapAdj.z));
            glm::vec3 fChildSize = glm::ceil(fSize / 2.0f);
            glm::uvec3 childSize(static_cast<unsigned int>(fChildSize.x), static_cast<unsigned int>(fChildSize.y), static_cast<unsigned int>(fChildSize.z));
            texSize = childSize;

            glm::uvec3 posOffsets[8];
            posOffsets[0] = glm::uvec3(0, 0, 0);
            posOffsets[1] = glm::uvec3(0, 0, 1);
            posOffsets[2] = glm::uvec3(0, 1, 0);
            posOffsets[3] = glm::uvec3(0, 1, 1);
            posOffsets[4] = glm::uvec3(1, 0, 0);
            posOffsets[5] = glm::uvec3(1, 0, 1);
            posOffsets[6] = glm::uvec3(1, 1, 0);
            posOffsets[7] = glm::uvec3(1, 1, 1);

            for (unsigned int i = 0; i < 8; ++i) {
                childs[i].reset(new VolumeBrickOctree(texData, pos + (posOffsets[i] * (childSize - glm::uvec3(1))),
                    childSize, minMaxMapProgram, minMaxMapUniformNames, app));
            }

            std::array<GLTexture*, 8> childTextures;
            for (unsigned int i = 0; i < 8; ++i) childTextures[i] = childs[i]->brickTexture.get();
            CombineChildTextures(app);
            WriteBrickTextureToTempFile();

            for (unsigned int i = 0; i < 8; ++i) childs[i]->ResetData();
        } else {
            brickTexture = texData->CreateMinMaxTexture(posOffset, origSize, brickTextureDesc);
            WriteBrickTextureToTempFile();
        }
    }


    VolumeBrickOctree::~VolumeBrickOctree()
    {
        if (streamFile) std::fclose(streamFile);
        streamFile = nullptr;
    }

    void VolumeBrickOctree::ReloadData()
    {
        std::fseek(streamFile, 0, SEEK_SET);
        std::vector<uint8_t> data;
        data.resize(dataSize);
        std::fread(data.data(), sizeof(uint8_t), dataSize, streamFile);

        brickTexture.reset(new GLTexture(origSize.x, origSize.y, origSize.z, brickTextureDesc, nullptr));
        brickTexture->UploadData(data);
        brickTexture->GenerateMinMaxMaps(minMaxMapProgram, minMaxMapUniformNames);
    }

    void VolumeBrickOctree::ResetData()
    {
        brickTexture.reset(nullptr);
    }

    void VolumeBrickOctree::WriteBrickTextureToTempFile()
    {
        std::vector<uint8_t> data;
        brickTexture->DownloadData(data);

        dataSize = static_cast<unsigned int>(data.size());
        streamFile = std::tmpfile();
        std::fwrite(data.data(), sizeof(uint8_t), dataSize, streamFile);
        ReloadData();
    }

    void VolumeBrickOctree::CombineChildTextures(ApplicationBase* app)
    {
        GPUProgram* combineProg = nullptr;
        brickTextureDesc.bytesPP = childs[0]->brickTextureDesc.bytesPP;
        brickTextureDesc.format = childs[0]->brickTextureDesc.format;
        brickTextureDesc.type = childs[0]->brickTextureDesc.type;
        brickTextureDesc.internalFormat = childs[0]->brickTextureDesc.internalFormat;

        if (brickTextureDesc.type == GL_UNSIGNED_BYTE) combineProg = app->GetGPUProgramManager()->GetResource("combineChildTextures8.cp");
        else if (brickTextureDesc.type == GL_UNSIGNED_SHORT) combineProg = app->GetGPUProgramManager()->GetResource("combineChildTextures16.cp");
        else if (brickTextureDesc.type == GL_UNSIGNED_INT) combineProg = app->GetGPUProgramManager()->GetResource("combineChildTextures32.cp");
        else throw std::runtime_error("Pixel-type not supported.");

        std::vector<BindingLocation> uniformNames = combineProg->GetUniformLocations(
            boost::assign::list_of<std::string>("childTex")("combineTex")("childShift")("maxChunkSize"));
        brickTexture.reset(new GLTexture(texSize.x, texSize.y, texSize.z, brickTextureDesc, nullptr));

        glm::uvec3 chunkSize = glm::uvec3(glm::ceil(glm::vec3(texSize) / 2.0f));
        combineProg->UseProgram();
        combineProg->SetUniform(uniformNames[0], 0);
        combineProg->SetUniform(uniformNames[1], 1);
        combineProg->SetUniform(uniformNames[3], chunkSize);

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
        for (unsigned int i = 0; i <= 8; ++i) {
            glm::ivec3 numGroups = glm::ivec3(glm::ceil(glm::vec3(chunkSize) / 8.0f));

            combineProg->SetUniform(uniformNames[2], childShifts[i]);
            childs[i]->brickTexture->ActivateImage(0, 0, GL_READ_ONLY);
            OGL_CALL(glDispatchCompute, numGroups.x, numGroups.y, numGroups.z);
        }

        OGL_CALL(glMemoryBarrier, GL_ALL_BARRIER_BITS);
        OGL_SCALL(glFinish);
    }
}
