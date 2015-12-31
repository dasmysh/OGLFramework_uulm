/**
* @file    CUDAGrid.cpp
* @author  Sebastian Maisch <sebastian.maisch@googlemail.com>
* @date    2014.04.02
*
* @brief   Implementation of the CUDA grid.
*/

#include "CUDAGrid.h"
#include "cudamain.h"
#include "gpgpu/CUDAImage.h"

#include <glm/glm.hpp>

namespace cgu {
    namespace gpgpu {

        /// <summary>
        /// Initializes a new instance of the <see cref="CUDAGrid"/> class.
        /// </summary>
        /// <param name="width">The width of the grid.</param>
        /// <param name="height">The height of the grid.</param>
        /// <param name="depth">The depth of the grid.</param>
        /// <param name="elementSize">Size of one element.</param>
        CUDAGrid::CUDAGrid(unsigned int width, unsigned int height, unsigned int depth, size_t elementSize) :
            size(make_cudaExtent(width * elementSize, height, depth)),
            dimensions(width, height, depth)
        {
            assert(width > 0);
            assert(height > 0);
            assert(depth > 0);
            CudaSafeCall(cudaMalloc(&grid, size.width * size.height * size.depth));
        }

        /// <summary>
        /// Finalizes an instance of the <see cref="CUDAGrid"/> class.
        /// </summary>
        CUDAGrid::~CUDAGrid()
        {
            CudaSafeCall(cudaFree(grid));
        }

        /// <summary>
        /// Initializes the grid.
        /// </summary>
        /// <param name="value">The value to initialize each field.</param>
        void CUDAGrid::InitGrid(int value) const
        {
            CudaSafeCall(cudaMemset(grid, value, size.width * size.height * size.depth));
        }

        /// <summary>
        /// Initializes the grid using a texture. Will only work for 2d grids.
        /// </summary>
        /// <param name="texture">The texture.</param>
        void CUDAGrid::InitGrid(CUDAImage* texture) const
        {
            assert(size.depth == 1);
            auto src = texture->GetMappedArray();
            CudaSafeCall(cudaMemcpy2DFromArray(grid, size.width, src, 0, 0, size.width,
                size.height, cudaMemcpyDeviceToDevice));
        }

        /// <summary>
        /// Copies the grids content to texture. Works only with 2d grids.
        /// </summary>
        /// <param name="texture">The texture.</param>
        void CUDAGrid::CopyToTexture(CUDAImage* texture) const
        {
            assert(size.depth == 1);
            auto dst = texture->GetMappedArray();
            CudaSafeCall(cudaMemcpyToArray(dst, 0, 0, grid, size.width * size.height,
                cudaMemcpyDeviceToDevice));
        }


        /// <summary>
        /// Copies grid contents to host.
        /// </summary>
        /// <param name="ptr">The pointer to allocated host memory.</param>
        /// <param name="destSize">Size of the host memory in elements.</param>
        /// <param name="elementSize">Size of a single element.</param>
        void CUDAGrid::CopyToHost(void* ptr, const glm::uvec3 destSize, size_t elementSize) const
        {
            assert(dimensions.x == destSize.x);
            assert(dimensions.y == destSize.y);
            assert(dimensions.z == destSize.z);
            assert(size.width == (destSize.x * elementSize));
            CudaSafeCall(cudaMemcpy(ptr, grid, size.width * size.height * size.depth,
                cudaMemcpyDeviceToHost));
        }
    }
}
