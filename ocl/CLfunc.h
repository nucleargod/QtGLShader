#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

typedef unsigned char pxType;

class CLMatTranspose{
public:
  CLMatTranspose();
  
  // step by step
  bool setInput(pxType *r, size_t cols, size_t rows);
  void transposeAsync();
  bool getOutput(pxType *output);
  
  //all-in-one
  bool transpose(pxType *output, pxType *r, size_t cols, size_t rows){
    if(!setInput(r, cols, rows)) return false;
    transposeAsync();
    return getOutput(output);
  }
  
  //get time usage
  double kernelTime();
  double memoryTime();
  
protected:
  cl_platform_id platform;
  cl_device_id device;
  cl_context context;
  cl_command_queue queue;
  cl_program program;
  cl_kernel matTransKernel;
  // for time
  cl_event event, event_m;
  // buffer
  size_t bsize, threads;
  cl_mem clSrc, clDst;
  
  bool buildKernel();
  
  static const char *kernelSource;
};
