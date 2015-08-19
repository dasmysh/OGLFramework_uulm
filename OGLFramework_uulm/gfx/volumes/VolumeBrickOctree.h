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
#include "gfx/glrenderer/GLVertexAttributeArray.h"
#include "gfx/glrenderer/GLTexture.h"

namespace cgu {

    class GLTexture3D;
    class GPUProgram;

    class VolumeBrickOctree
    {
    public:
        VolumeBrickOctree(const GLTexture3D* texData, const glm::uvec3& posOffset, const glm::uvec3& size,
            GPUProgram* minMaxProg, const std::vector<BindingLocation> uniformNames, ApplicationBase* app);
        ~VolumeBrickOctree();

    private:
        /** Holds the maximum resolution per brick. */
        static const unsigned int MAX_SIZE = 256;

        void WriteBrickTextureToTempFile();
        void ResetData();
        void ReloadData();
        void CombineChildTextures(ApplicationBase* app);

        /** Holds the position offset of this node. */
        const glm::uvec3 posOffset;
        /** Holds the original size of the volume (in this node). */
        const glm::uvec3 origSize;
        /** Holds the actual texture size of this node. */
        glm::uvec3 texSize;

        /** Holds the children in this layer. */
        std::array<std::unique_ptr<VolumeBrickOctree>, 8> childs;

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
