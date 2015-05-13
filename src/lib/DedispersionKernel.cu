#ifndef DEDISPERSE_KERNEL_H_
#define DEDISPERSE_KERNEL_H_
#include <assert.h>
#include <iostream>
#include "stdio.h"
#include "DedispersionParameters.h"

// Stores temporary shift values
//__device__ __constant__ float dm_shifts[8192];
//__device__ __constant__ int   i_nsamp, i_maxshift, i_nchans;
//__device__ __shared__ float f_line[ARRAYSIZE];


//{{{ global_for_time_dedisperse_loop
__global__ void cache_dedisperse_loop(float *outbuff, float *buff, float mstartdm,
                                      float mdmstep, const float* dm_shifts,
                                      const int i_nsamp, const int i_maxshift,
                                      const int i_nchans )
{

    int   shift;
    float local_kernel_t[NUMREG];

    int t  = blockIdx.x * NUMREG * DIVINT  + threadIdx.x;

    // Initialise the time accumulators
    for(int i = 0; i < NUMREG; i++) local_kernel_t[i] = 0.0f;

    float shift_temp = mstartdm + ((blockIdx.y * DIVINDM + threadIdx.y) * mdmstep);

    // Loop over the frequency channels.
    for(int c = 0; c < i_nchans; c++) {
        // Calculate the initial shift for this given frequency
        // channel (c) at the current despersion measure (dm) 
        // ** dm is constant for this thread!!**
        shift = (c * i_nsamp + t) + __float2int_rz (dm_shifts[c] * shift_temp);

        #pragma unroll
        for(int i = 0; i < NUMREG; i++) {
            local_kernel_t[i] += buff[shift + (i * DIVINT) ];
            //local_kernel_t[i] += __ldg(&buff[shift + (i * DIVINT) ]);
        }
    }

    // Write the accumulators to the output array. 
    #pragma unroll
    for(int i = 0; i < NUMREG; i++) {
        outbuff[((blockIdx.y * DIVINDM) + threadIdx.y)* (i_nsamp-i_maxshift) + (i * DIVINT) + (NUMREG * DIVINT * blockIdx.x) + threadIdx.x] = local_kernel_t[i];
    }
}

/// C Wrapper for brute-force algo
extern "C" void cacheDedisperseLoop( float *outbuff, long outbufSize, float *buff, float mstartdm,
                                     float mdmstep, int tdms, int numSamples,
                                     const float* dmShift,
                                     const int maxshift,
                                     const int i_nchans ) {

    cudaMemset(outbuff, 0, outbufSize );
    int divisions_in_t  = DIVINT;
    int divisions_in_dm = DIVINDM - (tdms%DIVINDM); // ensure divides exactly into
                                                    // our dm parameter space
    int num_reg = NUMREG;
    int num_blocks_t = (numSamples - maxshift)/(divisions_in_t * num_reg);
    int num_blocks_dm = tdms/divisions_in_dm;

/*
    std::cout << "\nnumSamples\t" << numSamples << std::endl;
    std::cout << "\ndivisions_in_t\t" << divisions_in_t << std::endl;
    std::cout << "\ndivisions_in_dm\t" << divisions_in_dm << std::endl;
    std::cout << "\nnum_reg\t" << num_reg << std::endl;
    std::cout << "\nnum_blocks_t\t" << num_blocks_t << std::endl;
    std::cout << "\nnum_blocks_dm\t" << num_blocks_dm << std::endl;
    std::cout << "\ntdms\t" << tdms << std::endl;
    std::cout << "mdmstep\t" << mdmstep << std::endl;
    std::cout << "mstartdm\t" << mstartdm << std::endl;
    std::cout << "buff\t" << buff << std::endl;
    std::cout << "outbuff\t" << outbuff << std::endl;
*/

    dim3 threads_per_block(divisions_in_t, divisions_in_dm);
    dim3 num_blocks(num_blocks_t,num_blocks_dm);

    cache_dedisperse_loop<<< num_blocks, threads_per_block >>>( outbuff, buff, 
                mstartdm, mdmstep, dmShift, numSamples, maxshift, i_nchans );
}

#endif
