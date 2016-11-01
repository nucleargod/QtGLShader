#include "CLfunc.h"
#include <cstdio>
#include <cstring>

// This will output the proper error strings in the event that a host call returns an error
#ifndef checkErrors
// #define checkCudaErrors(err)
#define checkErrors(err)  __checkErrors (err, __FILE__, __LINE__)
inline void __checkErrors(cl_int err, const char *file, const int line ){
    if(CL_SUCCESS != err){
        fprintf(stderr, "%s(%i) : CL Runtime API error %d: %s.\n",file , line, (int)err);
        exit(-1);        
    }
}
#endif

const char *CLMatTranspose::kernelSource = 
  "__kernel void matTrans                   \n"
  "(__global int *dst, __global int *src, const int yDimS, const int xDimS){\n"
  "  const int idxS = get_global_id(0);     \n"
  "  if(idxS >= xDimS) return;              \n"
  "  int rowLenS = xDimS, rowLenD = yDimS;  \n"
  "  int i, idS = idxS, idD = idxS*rowLenD; \n"
  "  for (i = 0; i<yDimS; i++){             \n"
  "    dst[idD  ] = src[idS  ];             \n" //one int has four byte RGBA
  "    idS += rowLenS; idD ++;              \n"
  "  }                                      \n"
  "}\n";

CLMatTranspose::CLMatTranspose() : bsize(0)
{
  cl_int error;
  
  /* Get the CL device */
  checkErrors( clGetPlatformIDs(1, &platform, NULL));
  checkErrors( clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL));
  printf("cl platform: %d, device: %d\n", platform, device);

  /* Initialize the CL device............. */
  context = clCreateContext(0, 1, &device, NULL, NULL, &error);
  queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &error);
  checkErrors(error);
  
  if(!buildKernel()){exit(-1);}
}

//*--- step by step
bool CLMatTranspose::setInput(pxType *r, size_t cols, size_t rows){
  cl_int error;
  bsize = cols*rows*4*sizeof(pxType);
  threads = cols;
  
  /* Initialize the CL device memory  */
  clSrc = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, bsize, r, &error);
  clDst = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bsize, NULL, &error);
  checkErrors(error);
  
  /* set args */
  checkErrors( clSetKernelArg(matTransKernel, 0, sizeof(cl_mem), &clDst ));
  checkErrors( clSetKernelArg(matTransKernel, 1, sizeof(cl_mem), &clSrc));
  checkErrors( clSetKernelArg(matTransKernel, 2, sizeof(int)   , &rows ));
  checkErrors( clSetKernelArg(matTransKernel, 3, sizeof(int)   , &cols ));
  
  return true;
}
void CLMatTranspose::transposeAsync(){
  size_t global[] = {threads};
  checkErrors( clEnqueueNDRangeKernel(queue, matTransKernel, 1, NULL, global, NULL, 0, NULL, &event));
}
bool CLMatTranspose::getOutput(pxType *output){
  checkErrors( clEnqueueReadBuffer(queue, clDst, CL_TRUE, 0, bsize, output, 0, NULL, &event_m));
  checkErrors( clReleaseMemObject(clSrc));
  checkErrors( clReleaseMemObject(clDst));
  checkErrors( clFinish(queue));
  return true;
}

//*--- get time usage
double CLMatTranspose::kernelTime(){
  cl_ulong cl_ts, cl_te;
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ts), &cl_ts, NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_te), &cl_te, NULL);
  return (double)(cl_te-cl_ts)/1e9;
}
double CLMatTranspose::memoryTime(){
  cl_ulong cl_ts_m, cl_te_m;
  clGetEventProfilingInfo(event_m, CL_PROFILING_COMMAND_START, sizeof(cl_ts_m), &cl_ts_m, NULL);
  clGetEventProfilingInfo(event_m, CL_PROFILING_COMMAND_END, sizeof(cl_te_m), &cl_te_m, NULL);
  return (double)(cl_te_m-cl_ts_m)/1e9;
}

//*--- protected utils
bool CLMatTranspose::buildKernel(){
  cl_int error;
  size_t file_size = strlen(kernelSource);

  program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, &file_size, &error);
  checkErrors(error);
  
  /* Prepare cl source done, let's bulid (compile) OpenCL program... */
  error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
  if (error != CL_SUCCESS) {
    size_t valueSize;
    char *value;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &valueSize);
    value = (char* )malloc((valueSize+1));
    value[valueSize] = '\0';
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, valueSize, value, NULL);
    fprintf(stderr, "build log :%s\n", value);
    free(value);
    return false;
  }
  
  matTransKernel = clCreateKernel(program, "matTrans", &error);
  checkErrors(error);
  return true;
}
