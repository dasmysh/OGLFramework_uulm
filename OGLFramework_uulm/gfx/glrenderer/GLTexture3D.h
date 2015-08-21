/**
 * @file   GLTexture3D.h
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.14
 *
 * @brief  Contains 3d texture class for openGL implementation.
 */

#ifndef GLTEXTURE3D_H
#define GLTEXTURE3D_H

#include "main.h"
#include "core/Resource.h"
#include "GLTexture.h"

namespace cgu {
    class VolumeBrickOctree;

    /**
     *  @brief 3D Texture for the OpenGL implementation.
     *
     * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
     * @date   2015.08.14
     */
    class GLTexture3D : public Resource
    {
    public:
        GLTexture3D(const std::string& texFilename, ApplicationBase* app);
        GLTexture3D(const GLTexture3D& orig) = delete;
        GLTexture3D(const GLTexture3D&& orig) = delete;
        GLTexture3D& operator=(const GLTexture3D& orig) = delete;
        GLTexture3D& operator=(const GLTexture3D&& orig) = delete;
        virtual ~GLTexture3D();

        virtual void Load() override;
        virtual void Unload() override;

        GLTexture* GetTexture();

        std::unique_ptr<VolumeBrickOctree> GetBrickedVolume();
        void FillRaw(std::vector<uint8_t>& data, const glm::uvec3& pos, const glm::uvec3& size) const;
        std::unique_ptr<GLTexture> CreateMinMaxTexture(const glm::uvec3& pos, const glm::uvec3& size, TextureDescriptor& minMaxDesc) const;
        const TextureDescriptor& GetTextureDescriptor() const;

    private:
        /** Holds the texture. */
        std::unique_ptr<GLTexture> texture;
        /** Holds the textures size. */
        glm::uvec3 volumeSize;
        /** Holds the size of each cell. */
        glm::vec3 cellSize;
        /** Holds the filename of the raw file. */
        std::string rawFileName;
        /** Holds a scale value for the stored data. */
        unsigned int scaleValue;
        /** Holds the texture description. */
        TextureDescriptor texDesc;
        /** Holds the open file stream. */
        std::ifstream* fileStream;
        /** Holds the volumes raw data. */
        std::vector<int8_t> data;

        void UnloadLocal();
    };
}

#endif // GLTEXTURE3D_H

