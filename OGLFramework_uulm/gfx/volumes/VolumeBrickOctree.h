/**
 * @file   VolumeBrickOctree.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.17
 *
 * @brief  Defines the layered octree for volume bricks.
 */

#ifndef VOLUMEBRICKOCTREE_H
#define VOLUMEBRICKOCTREE_H

#include "main.h"
#include "gfx/glrenderer/GLTexture.h"

namespace cgu {

    class GLTexture3D;
    class GPUProgram;
    class CameraView;

    class VolumeBrickOctree
    {
    public:
        VolumeBrickOctree(const GLTexture3D* texData, const glm::uvec3& posOffset, const glm::uvec3& size,
            const glm::vec3& scale, GPUProgram* minMaxProg, const std::vector<BindingLocation> uniformNames,
            ApplicationBase* app);
        ~VolumeBrickOctree();

        bool UpdateFrustum(const cgu::CameraView& camera, const glm::mat4& world);
        void GetRenderedBricksList(const cgu::CameraView& camera, const glm::mat4& world,
            std::vector<std::pair<const VolumeBrickOctree*, float>> &result) const;
        glm::mat4 GetLocalWorld(const glm::mat4& world) const;
        glm::vec3 GetWorldScale() const { return voxelScale * glm::vec3(origSize) * (maxTexValue - minTexValue); }
        // glm::vec3 GetWorldScale() const { return voxelScale * glm::vec3(origSize); }
        bool IsLoaded() const { return static_cast<bool>(brickTexture); }
        const GLTexture* GetTexture() const { return brickTexture.get(); }
        const glm::vec3& GetMinTexCoord() const { return minTexValue; }
        const glm::vec3& GetMaxTexCoord() const { return maxTexValue; }


        /** Holds the maximum resolution per brick. */
        static const unsigned int MAX_SIZE = 256;

    private:
        VolumeBrickOctree(const glm::uvec3& pos, const glm::uvec3& size, const glm::vec3& scale, unsigned int lvl,
            GPUProgram* minMaxProg, const std::vector<BindingLocation> uniformNames);
        VolumeBrickOctree(const GLTexture3D* texData, const glm::uvec3& posOffset, const glm::uvec3& size,
            unsigned int lvl, const glm::vec3& scale, GPUProgram* minMaxProg,
            const std::vector<BindingLocation> uniformNames, ApplicationBase* app);

        void CreateNode(const glm::uvec3& childSizeBase, const GLTexture3D* texData, ApplicationBase* app);
        void CreateLeafTexture(const GLTexture3D* texData);

        void CalculateTexBorders(const GLTexture3D* texData);

        void WriteBrickTextureToTempFile();
        void ResetData();
        void ResetAllData();
        void ReloadData();
        void CombineChildTextures(ApplicationBase* app);

        /** Holds the position offset of this node. */
        const glm::uvec3 posOffset;
        /** Holds the original size of the volume (in this node). */
        const glm::uvec3 origSize;
        /** Holds the scaling of a voxel. */
        const glm::vec3 voxelScale;
        /** Holds the current tree level. */
        unsigned int level;
        /** Holds the actual texture size of this node. */
        glm::uvec3 texSize;
        /** Holds whether this node or any of its children has data loaded (i.e. is not culled). */
        bool hasAnyData;
        /** Holds the minimum value in texture space (excluding overlap). */
        glm::vec3 minTexValue;
        /** Holds the maximum value in texture space (excluding overlap).*/
        glm::vec3 maxTexValue;
        /** Holds the maximum level of the current sub-tree. */
        unsigned int maxLevel;

        /** Holds the children in this layer. */
        std::array<std::unique_ptr<VolumeBrickOctree>, 8> children;

        /** Holds the temporary file with data for streaming. */
        std::FILE* streamFile;
        /** Holds the size of the data in the file. */
        unsigned int dataSize;
        /** Holds the texture descriptor. */
        TextureDescriptor brickTextureDesc;
        /** Holds the texture of this brick. */
        std::unique_ptr<GLTexture> brickTexture;
        /** Holds the GPUProgram for generating Min/Max MipMaps. */
        GPUProgram* minMaxMapProgram;
        /** Holds the binding locations for the Min/Max MipMaps. */
        std::vector<BindingLocation> minMaxMapUniformNames;
    };
}

#endif // VOLUMEBRICKOCTREE_H
