/**
 * @file   CUDAGrid.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.04.10
 *
 * @brief  Definition of the grid in cuda.
 */

#ifndef CUDAGRID_H
#define CUDAGRID_H

#include <cuda_runtime_api.h>
#include <glm/glm.hpp>

namespace cgu {
    namespace gpgpu {

        class CUDAImage;

        /// <summary>
        /// Initializes a grid in CUDA.
        /// </summary>
        class CUDAGrid
        {
        public:
            CUDAGrid(unsigned int width, unsigned int height, unsigned int depth, size_t elementSize);
            ~CUDAGrid();

            void InitGrid(int value) const;
            void InitGrid(CUDAImage* texture) const;
            void CopyToTexture(CUDAImage* texture) const;
            void CopyToHost(void* ptr, const glm::uvec3 destSize, size_t elementSize) const;

            const glm::uvec3& GetDimensions() const { return dimensions; };
            void* GetGridPtr() const { return grid; };

        private:
            /** holds the grids size. */
            cudaExtent size;
            /** holds the grids address on CUDA. */
            void* grid;
            /** holds the grids dimensions. */
            glm::uvec3 dimensions;
        };
    }
}

#endif /* CUDAGRID_H */
