/**
* @file    CUDAAddNoise.cu
* @author  Sebastian Maisch <sebastian.maisch@googlemail.com>
* @date    2014.04.02
*
* @brief   Implementation of the CUDA grid initialization.
*/

#include "CUDAAddNoise.h"
#include "cudamain.h"
#include "gpgpu/CUDAGrid.h"

#define GLM_FORCE_CUDA
#define GLM_SWIZZLE
#include <glm/glm.hpp>

#pragma warning (disable : 4505)
#ifndef __CUDACC__
    #define __CUDACC__
#endif
#include <curand_kernel.h>
#include <device_launch_parameters.h>
#include <device_functions.h>
#pragma warning (default : 4505)

namespace cgu {
    namespace gpgpu {

        __constant__ float grad[12][3] = { { 0, 1, 1 }, { 0, 1, -1 }, { 0, -1, 1 }, { 0, -1, -1 },
        { 1, 0, 1 }, { 1, 0, -1 }, { -1, 0, 1 }, { -1, 0, -1 },
        { 1, 1, 0 }, { 1, -1, 0 }, { -1, 1, 0 }, { -1, -1, 0 } };


        CUDAAddNoise::CUDAAddNoise(CUDAGrid* theGrid) :
            grid(theGrid)
        {
        }

        CUDAAddNoise::~CUDAAddNoise()
        {
        }

        __global__ void initKernel(glm::uvec3 dim, void* grid, unsigned long long seed,
            float resolution, float strength, glm::vec4 color);

        void CUDAAddNoise::RunKernel(const glm::vec4& color, float resolution, float strength, unsigned int seed) const
        {
            dim3 blocks(static_cast<unsigned int>(grid->GetDimensions().y),
                static_cast<unsigned int>(grid->GetDimensions().z));
            dim3 threads(static_cast<unsigned int>(grid->GetDimensions().x));
            initKernel<<<blocks, threads >>>(grid->GetDimensions(), grid->GetGridPtr(), seed, resolution, strength, color);
            CudaCheckError();
        }

        /*
        * Efficient simplex indexing functions by Bill Licea-Kane, ATI. Thanks!
        * (This was originally implemented as a 1D texture lookup. Nice to avoid that.)
        */
        __device__ void simplex(const glm::vec3& P, glm::vec3& offset1, glm::vec3& offset2)
        {
            glm::vec3 offset0;

            auto isX = glm::step(glm::vec2(P.y, P.z), glm::vec2(P.x, P.x)); // P.x >= P.y ? 1.0 : 0.0;  P.x >= P.z ? 1.0 : 0.0;
            // Accumulate all P.x >= other channels in offset.x
            offset0 = glm::vec3(isX.x + isX.y, 1.0f - isX);        // Accumulate all P.x <  other channels in offset.yz

            auto v_isY = glm::step(glm::fvec1(P.z), glm::fvec1(P.y));  // P.y >= P.z ? 1.0 : 0.0;
            auto isY = v_isY.x;
            offset0.y += isY;              // Accumulate P.y >= P.z in offset.y
            offset0.z += 1.0 - isY;        // Accumulate P.y <  P.z in offset.z

            // offset0 now contains the unique values 0,1,2 in each channel
            // 2 for the channel greater than other channels
            // 1 for the channel that is less than one but greater than another
            // 0 for the channel less than other channels
            // Equality ties are broken in favor of first x, then y
            // (z always loses ties)

            offset2 = glm::clamp(offset0, 0.0f, 1.0f);
            // offset2 contains 1 in each channel that was 1 or 2
            offset1 = glm::clamp(offset0 - 1.0f, 0.0f, 1.0f);
            // offset1 contains 1 in the single channel that was 1
        }

        // , glm::vec3* grad3
        __global__ void initKernel(glm::uvec3 dim, void* grid, unsigned long long seed,
            float resolution, float strength, glm::vec4 color)
        {
            const float F3 = 0.333333333333;
            const float G3 = 0.166666666667;

            // const float ONE = 1.0f / blockDim.x;
            // const float ONEHALF = 0.5f / blockDim.x;
            // const unsigned int gridDimX = blockDim.x;
            const auto gridDimY = gridDim.x;
            // const unsigned int gridDimZ = gridDim.y;
            const auto gridX = threadIdx.x;
            const auto gridY = blockIdx.x;
            const auto gridZ = blockIdx.y;

            // printf("gridPos: (%d,%d,%d)\n", gridX, gridY, gridZ);
            // printf("gridDim: (%d,%d,%d)\n", gridDimX, gridDimY, gridDimZ);

            glm::vec3 P(static_cast<float>(gridX) / resolution,
                static_cast<float>(gridY) / resolution,
                static_cast<float>(gridZ) / resolution);
            // Skew the (x,y,z) space to determine which cell of 6 simplices we're in
            auto s = (P.x + P.y + P.z) * F3; // Factor for 3D skewing
            auto Pi = glm::floor(P + s);
            auto t = (Pi.x + Pi.y + Pi.z) * G3;
            auto P0 = Pi - t; // Unskew the cell origin back to (x,y,z) space
            // Pi = Pi * ONE + ONEHALF; // Integer part, scaled and offset for texture lookup

            auto Pf0 = P - P0;  // The x,y distances from the cell origin

            // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
            // To find out which of the six possible tetrahedra we're in, we need to
            // determine the magnitude ordering of x, y and z components of Pf0.
            glm::vec3 o1;
            glm::vec3 o2;
            simplex(Pf0, o1, o2);

            curandStateXORWOW_t rand_state;
            auto uPi = glm::uvec3(__float2uint_rd(Pi.x), __float2uint_rd(Pi.y), __float2uint_rd(Pi.z));
            // glm::uvec3 uo1 = glm::uvec3(__float2uint_rd(o1.x), __float2uint_rd(o1.y), __float2uint_rd(o1.z));
            // glm::uvec3 uo2 = glm::uvec3(__float2uint_rd(o2.x), __float2uint_rd(o2.y), __float2uint_rd(o2.z));
            curand_init(seed + uPi.x, uPi.y, uPi.z, &rand_state);
            auto perm0 = curand_uniform(&rand_state);
            curand_init(seed + uPi.x + o1.x, uPi.y + o1.y, uPi.z + o1.z, &rand_state);
            auto perm1 = curand_uniform(&rand_state);
            curand_init(seed + uPi.x + o2.x, uPi.y + o2.y, uPi.z + o2.z, &rand_state);
            auto perm2 = curand_uniform(&rand_state);
            curand_init(seed + uPi.x + 1, uPi.y + 1, uPi.z + 1, &rand_state);
            auto perm3 = curand_uniform(&rand_state);

            // Noise contribution from simplex origin
            // float perm0 = 1texture2D(gradTexture, Pi.xy).a;
            // glm::vec3  grad0 = 2texture2D(gradTexture, glm::vec2(perm0, Pi.z)).rgb * 4.0 - 2.0;
            auto pgrad0 = grad[static_cast<unsigned int>(12.0f * perm0)];
            glm::vec3 grad0(pgrad0[0], pgrad0[1], pgrad0[2]);
            float t0 = 0.6 - glm::dot(Pf0, Pf0);
            float n0;
            if (t0 < 0.0) n0 = 0.0;
            else {
                t0 *= t0;
                n0 = t0 * t0 * glm::dot(grad0, Pf0);
            }

            // Noise contribution from second corner
            auto Pf1 = Pf0 - o1 + G3;
            // float perm1 = 3texture2D(gradTexture, Pi.xy + o1.xy*ONE).a;
            // glm::vec3  grad1 = 4texture2D(gradTexture, glm::vec2(perm1, Pi.z + o1.z*ONE)).rgb * 4.0 - 2.0;
            auto pgrad1 = grad[static_cast<unsigned int>(12.0f * perm1)];
            glm::vec3 grad1(pgrad1[0], pgrad1[1], pgrad1[2]);
            float t1 = 0.6 - glm::dot(Pf1, Pf1);
            float n1;
            if (t1 < 0.0) n1 = 0.0;
            else {
                t1 *= t1;
                n1 = t1 * t1 * glm::dot(grad1, Pf1);
            }

            // Noise contribution from third corner
            auto Pf2 = Pf0 - o2 + 2.0f * G3;
            // float perm2 = 5texture2D(gradTexture, Pi.xy + o2.xy*ONE).a;
            // glm::vec3  grad2 = 6texture2D(gradTexture, glm::vec2(perm2, Pi.z + o2.z*ONE)).rgb * 4.0 - 2.0;
            auto pgrad2 = grad[static_cast<unsigned int>(12.0f * perm2)];
            glm::vec3 grad2(pgrad2[0], pgrad2[1], pgrad2[2]);
            float t2 = 0.6 - glm::dot(Pf2, Pf2);
            float n2;
            if (t2 < 0.0) n2 = 0.0;
            else {
                t2 *= t2;
                n2 = t2 * t2 * glm::dot(grad2, Pf2);
            }

            // Noise contribution from last corner
            auto Pf3 = Pf0 - glm::vec3(1.0 - 3.0*G3);
            // float perm3 = 7texture2D(gradTexture, Pi.xy + glm::vec2(ONE, ONE)).a;
            // glm::vec3  grad3 = 8texture2D(gradTexture, glm::vec2(perm3, Pi.z + ONE)).rgb * 4.0 - 2.0;
            auto pgrad3 = grad[static_cast<unsigned int>(12.0f * perm3)];
            glm::vec3 grad3(pgrad3[0], pgrad3[1], pgrad3[2]);
            float t3 = 0.6 - glm::dot(Pf3, Pf3);
            float n3;
            if (t3 < 0.0) n3 = 0.0;
            else {
                t3 *= t3;
                n3 = t3 * t3 * glm::dot(grad3, Pf3);
            }

            auto gridPtr = static_cast<char*>(grid);
            auto pitch = dim.x * sizeof(glm::vec4);
            auto slicePitch = pitch * gridDimY;

            auto slicePtr = gridPtr + (gridZ * slicePitch);
            auto linePtr = reinterpret_cast<glm::vec4*>(slicePtr + (gridY * pitch));
            // glm::vec4* gridPos = &gridPtr[(gridZ * slicePitch) + (gridY * pitch) + gridX];
            auto gridPos = &linePtr[gridX];
            // scale result to [0,1] and adjust to strength and color
            // gridPtr[(gridZ * slicePitch) + (gridY * pitch) + gridX] =
            //     color * strength * ((16.0f * (n0 + n1 + n2 + n3)) + 0.8f);
            *gridPos += color * strength * ((16.0f * (n0 + n1 + n2 + n3)) + 0.5f);

            // *gridPos = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}