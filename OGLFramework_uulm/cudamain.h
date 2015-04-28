/**
 * @file    cudamain.h
 * @author  Sebastian Maisch
 * @date    2014.04.10
 *
 * @brief Includes common headers and defines used for cuda.
 */

#ifndef CUDAMAIN_H
#define CUDAMAIN_H

#include <cuda.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <stdexcept>

#include "core/cudaLogger.h"

///
/// CUDA Error checking. Stolen from http://choorucode.com/2011/03/02/how-to-do-error-checking-in-cuda/
///
#define CudaSafeCall( err ) __cudaSafeCall( err, __FILE__, __LINE__ )
#define CudaCheckError() __cudaCheckError( __FILE__, __LINE__ )

#ifdef _DEBUG

inline void __cudaSafeCall(cudaError err, const char *file, const int line)
{
    if ( cudaSuccess != err )
    {
        cudaLog::log(file, line) << L"cudaSafeCall() failed: " << cudaGetErrorString(err);
        throw std::runtime_error("CUDA Error: " + err);
    }
}

inline void __cudaCheckError( const char *file, const int line )
{
    cudaError err = cudaGetLastError();
    if ( cudaSuccess != err )
    {
        cudaLog::log(file, line) << L"cudaCheckError() failed: " << cudaGetErrorString(err);
        throw std::runtime_error("CUDA Error: " + err);
    }
 
    err = cudaDeviceSynchronize();
    if( cudaSuccess != err )
    {
        cudaLog::log(file, line) << L"cudaCheckError() (sync) failed: " << cudaGetErrorString(err);
        throw std::runtime_error("CUDA Error (sync): " + err);
    }
}

#else

inline void __cudaSafeCall(cudaError, const char*, const int)
{
    return;
}

inline void __cudaCheckError(const char*, const int)
{
    return;
}

#endif

#ifdef __CUDA_ARCH__
#define CONSTANT __constant__
#else
#define CONSTANT
#endif

static inline const char* cublasGetErrorString(cublasStatus_t status)
{
    switch(status)
    {
        case CUBLAS_STATUS_SUCCESS: return "CUBLAS_STATUS_SUCCESS";
        case CUBLAS_STATUS_NOT_INITIALIZED: return "CUBLAS_STATUS_NOT_INITIALIZED";
        case CUBLAS_STATUS_ALLOC_FAILED: return "CUBLAS_STATUS_ALLOC_FAILED";
        case CUBLAS_STATUS_INVALID_VALUE: return "CUBLAS_STATUS_INVALID_VALUE"; 
        case CUBLAS_STATUS_ARCH_MISMATCH: return "CUBLAS_STATUS_ARCH_MISMATCH"; 
        case CUBLAS_STATUS_MAPPING_ERROR: return "CUBLAS_STATUS_MAPPING_ERROR";
        case CUBLAS_STATUS_EXECUTION_FAILED: return "CUBLAS_STATUS_EXECUTION_FAILED"; 
        case CUBLAS_STATUS_INTERNAL_ERROR: return "CUBLAS_STATUS_INTERNAL_ERROR"; 
        case CUBLAS_STATUS_NOT_SUPPORTED: return "CUBLAS_STATUS_NOT_SUPPORTED";
    }
    return "unknown error";
}

#define CuBLASSafeCall( err ) __cublasSafeCall( err, __FILE__, __LINE__ )

#ifdef _DEBUG

inline  void __cublasSafeCall(cublasStatus_t err, const char *file, const int line)
{
    if ( CUBLAS_STATUS_SUCCESS != err )
    {
        cudaLog::log(file, line) << L"cuBLASSafeCall() failed: " << cublasGetErrorString(err);
        throw std::runtime_error("cuBLAS Error: " + err);
    }
}

#else

inline void __cublasSafeCall(cublasStatus_t, const char*, const int)
{
    return;
}

#endif

#endif /* CUDAMAIN_H */