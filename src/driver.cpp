/* SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause */
/*
 * Copyright 2023 Amazon.com, Inc. or its affiliates. All rights reserved.
 * Copyright 2024 IIC-SIG-MLsys(SDU), jacalau. All rights reserved.
 */

#include <hddt.h>

namespace hddt {

#ifdef ENABLE_CUDA
// cuda device init
status_t cuda_init(int device_id) {
  // used to count the device numbers
  int count;

  // get the cuda device count
  cudaGetDeviceCount(&count);
  if (count == 0) {
    logError("There is no device.\n");
    return status_t::ERROR;
  }

  // find the device >= 1.X
  int i;
  for (i = 0; i < count; ++i) {
    cudaDeviceProp prop;
    if (cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
      if (prop.major >= 1) {
        break;
      }
    }
  }

  // if can't find the device
  if (i == count) {
    logError("There is no device supporting CUDA 1.x.\n");
    return status_t::ERROR;
  }

  if (device_id <= count) {
    // set cuda device
    cudaSetDevice(i);
  } else {
    logError("Device id does not be supported.\n");
    return status_t::ERROR;
  }

  return status_t::SUCCESS;
}

#endif

#ifdef ENABLE_ROCM
// rocm driver init
status_t rocm_init(int device_id) {
  int deviceCount = 0;
  hipError_t error = hipGetDeviceCount(&deviceCount);

  if (error != hipSuccess) {
    printf("hipDeviceGetCount() returned %d\n", error);
    return status_t::ERROR;
  }

  if (device_id >= deviceCount) {
    printf("Requested ROCm device %d but found only %d device(s)\n", device_id,
           deviceCount);
    return status_t::ERROR;
  }

  error = hipSetDevice(device_id);
  if (error != hipSuccess)
    return status_t::ERROR;

  hipDeviceProp_t prop = {0};
  error = hipGetDeviceProperties(&prop, device_id);
  if (error != hipSuccess)
    return status_t::ERROR;

  /* Need 256 bytes to silence compiler warning */
  char archName[256];
#if HIP_VERSION >= 60000000
  snprintf(archName, 256, "%s", prop.gcnArchName);
#else
  snprintf(archName, 256, "%d", prop.gcnArch);
#endif

  printf("Using ROCm Device with ID: %d, Name: %s, PCI Bus ID: 0x%x, GCN Arch: "
         "%s\n",
         device_id, prop.name, prop.pciBusID, archName);

  return status_t::SUCCESS;
}

#endif

status_t init_gpu_driver(int device_id) {
  status_t ret = status_t::SUCCESS;
#ifdef ENABLE_CUDA
  ret = cuda_init(device_id);
#endif
#ifdef ENABLE_ROCM
  ret = rocm_init(device_id);
#endif
  return ret;
}

status_t free_gpu_driver() {
#ifdef ENABLE_CUDA
  printf("destroying current CUDA Ctx\n");
  // todo
#endif
  return status_t::SUCCESS;
}

} // namespace hddt
