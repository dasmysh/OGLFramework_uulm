/**
 * @file   CUDAImage.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.04.02
 *
 * @brief  Definition of the CUDA image class.
 */

#ifndef CUDAIMAGE_H
#define CUDAIMAGE_H

#include <cuda_runtime_api.h>

namespace cgu {

    class GLTexture;

    namespace gpgpu {

        /**
         * @brief Class for registering OpenGL textures with CUDA.<br/>
         *
         * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
         * @date   2014.04.02
         */
        class CUDAImage {
        public:
            CUDAImage(GLTexture* glTex, unsigned int flags);
            ~CUDAImage();

            void MapImage();
            void UnmapImage();
            cudaArray* GetMappedArray(unsigned int arrayIndex = 0, unsigned int mipLevel = 0) const;

        private:
            /** holds the opengl texture object. */
            GLTexture* texture;
            /** holds the cuda resource for the texture. */
            cudaGraphicsResource* cudaResource;
            /** holds if the resource is currently mapped. */
            bool mapped;
        };
    }
}

#endif /* CUDAIMAGE_H */
