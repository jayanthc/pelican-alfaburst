include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# === Create library of pipelines.
set(pipeline_lib_src
    src/EmptyPipeline.cpp
)
if(CUDA_FOUND)
    list(APPEND pipeline_lib_src
        src/SigprocPipeline.cpp
        src/ABPipeline.cpp
    )
endif(CUDA_FOUND)
add_library(pelicanMdsm ${pipeline_lib_src})

# === Build the Empty Pipeline for max performance testing
add_executable(emptyPipeline src/emptyPipelineMain.cpp)
target_link_libraries(emptyPipeline
    pelicanMdsm
    pelican-lofar_static
    ${PELICAN_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)
install(TARGETS emptyPipeline
		DESTINATION ${BINARY_INSTALL_DIR})

# === Build the ALFABURST pipeline binary
if(CUDA_FOUND)
add_executable(ABPipeline src/ABPipelineMain.cpp)
set_target_properties(ABPipeline PROPERTIES
    COMPILE_FLAGS "${OpenMP_CXX_FLAGS}"
    LINK_FLAGS "${OpenMP_CXX_FLAGS}"
)
target_link_libraries(ABPipeline
    pelicanMdsm
    pelican-lofar_static
    ${PELICAN_LIBRARY}
    ${FFTW3_FFTW_LIBRARY}
    ${FFTW3_FFTWF_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)
install(TARGETS ABPipeline
		DESTINATION ${BINARY_INSTALL_DIR})
endif(CUDA_FOUND)

# === Build the SIGPROC Pipeline
if(CUDA_FOUND)
add_executable(SigprocPipeline src/SigprocPipelineMain.cpp)
set_target_properties(SigprocPipeline PROPERTIES
    COMPILE_FLAGS "${OpenMP_CXX_FLAGS}"
    LINK_FLAGS "${OpenMP_CXX_FLAGS}"
)
target_link_libraries(SigprocPipeline
    pelicanMdsm
    pelican-lofar_static
    ${PELICAN_LIBRARY}
    ${FFTW3_FFTW_LIBRARY}
    ${FFTW3_FFTWF_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)
install(TARGETS SigprocPipeline
		DESTINATION ${BINARY_INSTALL_DIR})
endif(CUDA_FOUND)

include(CopyFiles)
copy_files(${CMAKE_CURRENT_SOURCE_DIR}/data/*.xml . mdsmXmlFiles)

add_subdirectory(test)
