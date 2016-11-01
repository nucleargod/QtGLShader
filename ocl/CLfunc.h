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
  bool setInput(pxType *r, size_t cols, size_t rows, size_t nc);
  void transposeAsync();
  bool getOutput(pxType *output);
  
  //all-in-one
  bool transpose(pxType *output, pxType *r, size_t cols, size_t rows, size_t nc){
    if(!setInput(r, cols, rows, nc)) return false;
    transposeAsync();
    if(!getOutput(output)) return false;
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
  size_t bsize;
  cl_mem clSrc, clDst;
  
  bool buildKernel();
  
  static const char *kernelSource;
};
