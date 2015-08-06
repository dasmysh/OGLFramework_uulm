/**
 * @file    CUDAImage.cpp
 * @author  Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date    2014.04.02
 *
 * @brief   Implementation of the CUDA image class.
 */

#include "CUDAImage.h"
#include "cudamain.h"
#include "gfx/glrenderer/GLTexture.h"
#include "gfx/glrenderer/GLUniformBuffer.h"

#include <cuda_gl_interop.h>

namespace cgu {
    namespace gpgpu {

        /// <summary>
        /// Initializes a new instance of the <see cref="CUDAImage"/> class.
        /// </summary>
        /// <param name="glTex">The opengl texture.</param>
        /// <param name="flags">The cuda flags.</param>
        CUDAImage::CUDAImage(GLTexture* glTex, unsigned int flags) :
            texture(glTex),
            cudaResource(nullptr),
            mapped(false)
        {
            CudaSafeCall(cudaGraphicsGLRegisterImage(&cudaResource, texture->GetGLIdentifier().textureId,
                texture->GetGLIdentifier().textureType, flags));
        }

        /// <summary>
        /// Finalizes an instance of the <see cref="CUDAImage"/> class.
        /// </summary>
        CUDAImage::~CUDAImage()
        {
            CudaSafeCall(cudaGraphicsUnregisterResource(cudaResource));
        }

        /// <summary>
        /// Maps the image into CUDA.
        /// </summary>
        void CUDAImage::MapImage()
        {
            if (mapped) return;
            CudaSafeCall(cudaGraphicsMapResources(1, &cudaResource));
            mapped = true;
        }

        /// <summary>
        /// Unmaps the image.
        /// </summary>
        void CUDAImage::UnmapImage()
        {
            if (!mapped) return;
            CudaSafeCall(cudaGraphicsUnmapResources(1, &cudaResource));
            mapped = false;
        }


        /// <summary>
        /// Returns the internal array of the mapped image.
        /// </summary>
        /// <param name="arrayIndex">Index of the array.</param>
        /// <param name="mipLevel">The mip level.</param>
        /// <returns></returns>
        cudaArray* CUDAImage::GetMappedArray(unsigned int arrayIndex, unsigned int mipLevel)
        {
            assert(mapped);
            cudaArray* result;
            CudaSafeCall(cudaGraphicsSubResourceGetMappedArray(&result, cudaResource, arrayIndex, mipLevel));
            return result;
        }
    }
}
