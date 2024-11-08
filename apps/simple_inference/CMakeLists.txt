# apps/simple_inference/CMakeLists.txt

project(simple_inference)

set(SIMPLE_INFERENCE_SOURCES
    main.cpp
)

add_executable(simple_inference ${SIMPLE_INFERENCE_SOURCES})

if(ROCM_FOUND)
    target_link_libraries(simple_inference PRIVATE hddt_rocm_shared)
elseif(CUDA_FOUND)
    target_link_libraries(simple_inference PRIVATE hddt_cuda_shared)
else()
    message(FATAL_ERROR "ROCm or CUDA is required but was not found.")
endif()

target_include_directories(simple_inference PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

include(FindMPI)
include_directories(${MPI_INCLUDE_DIR})
target_link_libraries(simple_inference PRIVATE ${MPI_LIBRARIES})

# find mpixx
find_program(MPICC mpicxx REQUIRED)
set(CMAKE_C_COMPILER ${MPICC})
set(CMAKE_CXX_COMPILER ${MPICC})

# install(TARGETS simple_inference DESTINATION bin)