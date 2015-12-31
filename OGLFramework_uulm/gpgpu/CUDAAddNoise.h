/**
 * @file   CUDAAddNoise.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.04.10
 *
 * @brief  Definition of a grid modifier that adds noise to the grid.
 */

#ifndef CUDAADDNOISE_H
#define CUDAADDNOISE_H

#include <glm/glm.hpp>

namespace cgu {
    namespace gpgpu {

        class CUDAGrid;

        /// <summary>
        /// Class for adding noise to a grid.
        /// </summary>
        class CUDAAddNoise
        {
        public:
            explicit CUDAAddNoise(CUDAGrid* theGrid);
            ~CUDAAddNoise();

            void RunKernel(const glm::vec4& color, float resolution, float strength, unsigned int seed) const;

        private:
            /** holds the grid. */
            CUDAGrid* grid;
        };
    }
}

#endif /* CUDAADDNOISE_H */
