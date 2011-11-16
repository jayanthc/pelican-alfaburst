#ifndef GPU_NVIDIA_H
#define GPU_NVIDIA_H

#ifdef CUDA_FOUND

#include "GPU_Resource.h"
#include <cuda.h>
#include <cuda_runtime_api.h>

/**
 * @file GPU_NVidia.h
 */

namespace pelican {

namespace lofar {
class GPU_Manager;

/**
 * @class GPU_NVidia
 *  
 * @brief
 *     An NVidia GPU Card abstraction for use with a GPU_Manager
 * @details
 * 
 */

class GPU_NVidia : public GPU_Resource
{

    public:
        GPU_NVidia( unsigned int id );
        ~GPU_NVidia();
        virtual void run( const GPU_Job& job );

        static void initialiseResources(GPU_Manager* manager);

    private:
        cudaDeviceProp _deviceProp;
};

} // namespace lofar
} // namespace pelican
#endif // CUDA_FOUND
#endif // GPU_NVIDIA_H 