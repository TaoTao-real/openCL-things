//
//  openclMatrixMulPro.cpp
//  matrixMul
//
//  Created by 李圣韬 on 2020/3/23.
//  Copyright © 2020 李圣韬. All rights reserved.
//

#include "openclMatrixMulPro.hpp"

using namespace std;

void openclMatrixMulPro(int n){
    const char* programSource =
    "__kernel                                                         \n"
    "void matrixMulPro(__global float* A,                             \n"
    "                  __global float* B,                             \n"
    "                  __global float* C,                             \n"
    "                  int size)                                      \n"
    "{                                                                \n"
    "    int row = get_local_id(0);                                   \n"
    "    int col = get_local_id(1);                                   \n"
    "    int globalrow = 16*get_group_id(0);                          \n"
    "    int globalcol = 16*get_group_id(1);                          \n"
    "    __local float subA[16][16];                                  \n"
    "    __local float subB[16][16];                                  \n"
    "    int tilesnum = size/16;                                      \n"
    "    float sum = 0;                                               \n"
    "    for(int k = 0; k < tilesnum; ++k){                           \n"
    "        subA[row][col] = A[size*(globalrow+row)+col+k*16];       \n"
    "        subB[row][col] = B[size*(row+k*16)+globalcol+col];       \n"
    "        barrier(CLK_LOCAL_MEM_FENCE);                            \n"
    "        for(int i = 0; i < 16; ++i){                             \n"
    "            sum += subA[row][i]*subB[i][col];                    \n"
    "        }                                                        \n"
    "        barrier(CLK_LOCAL_MEM_FENCE);                            \n"
    "    }                                                            \n"
    "    C[(globalrow+row)*size + globalcol + col] = sum;             \n"
    "}                                                                \n"
    ;
    
    // This code executes on the OpenCL host
    int N = n;
    // Host data
    float *A = NULL;  // Input array
    float *B = NULL;  // Input array
    float *C = NULL;  // Output array
    float *ANS = NULL;
    
    // Elements in each array
    int elements = N * N;
    
    // Compute the size of the data
    size_t datasize = sizeof(float)*elements;

    // Allocate space for input/output data
    A = (float*)malloc(datasize);
    B = (float*)malloc(datasize);
    C = (float*)malloc(datasize);
    ANS = (float*)malloc(datasize);
    
    // Initialize the input data
    for(int i = 0; i < elements; i++) {
        A[i] = 1;
        B[i] = 1;
        C[i] = 0;
    }
    
    
    // Use this to check the output of each API call
    cl_int status;
     
    //-----------------------------------------------------
    // STEP 1: Discover and initialize the platforms
    //-----------------------------------------------------
    
    cl_uint numPlatforms = 0;
    cl_platform_id *platforms = NULL;
    
    // Use clGetPlatformIDs() to retrieve the number of
    // platforms
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
 
    // Allocate enough space for each platform
    platforms = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
 
    // Fill in platforms with clGetPlatformIDs()
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);

    //-----------------------------------------------------
    // STEP 2: Discover and initialize the devices
    //-----------------------------------------------------
    
    cl_uint numDevices = 0;
    cl_device_id *devices = NULL;

    // Use clGetDeviceIDs() to retrieve the number of
    // devices present
    status = clGetDeviceIDs(
        platforms[0],
        CL_DEVICE_TYPE_GPU,
        0,
        NULL,
        &numDevices);

    // Allocate enough space for each device
    devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));

    // Fill in devices with clGetDeviceIDs()
    status = clGetDeviceIDs(
        platforms[0],
        CL_DEVICE_TYPE_GPU,
        numDevices,
        devices,
        NULL);

    //-----------------------------------------------------
    // STEP 3: Create a context
    //-----------------------------------------------------
    
    cl_context context = NULL;

    // Create a context using clCreateContext() and
    // associate it with the devices
    context = clCreateContext(
        NULL,
        numDevices,
        devices,
        NULL,
        NULL,
        &status);

    //-----------------------------------------------------
    // STEP 4: Create a command queue
    //-----------------------------------------------------
    cl_queue_properties_APPLE properties[] {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};

    // Create a command queue using clCreateCommandQueue(),
    // and associate it with the device you want to execute
    // on
    cl_command_queue cmdQueue = clCreateCommandQueueWithPropertiesAPPLE(
        context,
        devices[0],
        properties,
        &status);

    //-----------------------------------------------------
    // STEP 5: Create device buffers
    //-----------------------------------------------------
    
    cl_mem bufferA;  // Input array on the device
    cl_mem bufferB;  // Input array on the device
    cl_mem bufferC;  // Output array on the device

    // Use clCreateBuffer() to create a buffer object (d_A)
    // that will contain the data from the host array A
    bufferA = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        datasize,
        NULL,
        &status);

    // Use clCreateBuffer() to create a buffer object (d_B)
    // that will contain the data from the host array B
    bufferB = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        datasize,
        NULL,
        &status);

    // Use clCreateBuffer() to create a buffer object (d_C)
    // with enough space to hold the output data
    bufferC = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY,
        datasize,
        NULL,
        &status);
    
    //-----------------------------------------------------
    // STEP 6: Write host data to device buffers
    //-----------------------------------------------------
    
    // Use clEnqueueWriteBuffer() to write input array A to
    // the device buffer bufferA
    status = clEnqueueWriteBuffer(
        cmdQueue,
        bufferA,
        CL_FALSE,
        0,
        datasize,
        A,
        0,
        NULL,
        NULL);
    
    // Use clEnqueueWriteBuffer() to write input array B to
    // the device buffer bufferB
    status = clEnqueueWriteBuffer(
        cmdQueue,
        bufferB,
        CL_FALSE,
        0,
        datasize,
        B,
        0,
        NULL,
        NULL);

    //-----------------------------------------------------
    // STEP 7: Create and compile the program
    //-----------------------------------------------------
     
    // Create a program using clCreateProgramWithSource()
    cl_program program = clCreateProgramWithSource(
        context,
        1,
        (const char**)&programSource,
        NULL,
        &status);
    
    // Build (compile) the program for the devices with
    // clBuildProgram()
    status = clBuildProgram(
        program,
        numDevices,
        devices,
        NULL,
        NULL,
        NULL);
   
    //-----------------------------------------------------
    // STEP 8: Create the kernel
    //-----------------------------------------------------

    cl_kernel kernel = clCreateKernel(program, "matrixMulPro", &status);

    //-----------------------------------------------------
    // STEP 9: Set the kernel arguments
    //-----------------------------------------------------
    
    // Associate the input and output buffers with the
    // kernel
    // using clSetKernelArg()
    status  = clSetKernelArg(
        kernel,
        0,
        sizeof(cl_mem),
        &bufferA);
    status |= clSetKernelArg(
        kernel,
        1,
        sizeof(cl_mem),
        &bufferB);
    status |= clSetKernelArg(
        kernel,
        2,
        sizeof(cl_mem),
        &bufferC);
    status |= clSetKernelArg(
        kernel,
        3,
        sizeof(cl_mem),
        &N);

    //-----------------------------------------------------
    // STEP 10: Configure the work-item structure
    //-----------------------------------------------------
    
    // Define an index space (global work size) of work
    // items for
    // execution. A workgroup size (local work size) is not
    // required,
    // but can be used.
    size_t globalWorkSize[2];
    // There are 'elements' work-items
    globalWorkSize[0] = N;
    globalWorkSize[1] = N;
    size_t localWorkSize[2];
    localWorkSize[0] = 16;
    localWorkSize[1] = 16;
    //-----------------------------------------------------
    // STEP 11: Enqueue the kernel for execution
    //-----------------------------------------------------
    
    cl_event event;
    // Execute the kernel by using
    // clEnqueueNDRangeKernel().
    // 'globalWorkSize' is the 1D dimension of the
    // work-items
    status = clEnqueueNDRangeKernel(
        cmdQueue,
        kernel,
        2,
        NULL,
        globalWorkSize,
        localWorkSize,
        0,
        NULL,
        &event);
    clWaitForEvents(1, &event);
    cl_ulong start, end;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    double total_time = end - start;
    cout << "LM GPU用时： "  << (int)(total_time/1000000) << "ms. ";
    //-----------------------------------------------------
    // STEP 12: Read the output buffer back to the host
    //-----------------------------------------------------
    
    // Use clEnqueueReadBuffer() to read the OpenCL output
    // buffer (bufferC)
    // to the host output array (C)
    clEnqueueReadBuffer(
        cmdQueue,
        bufferC,
        CL_TRUE,
        0,
        datasize,
        C,
        0,
        NULL,
        NULL);
    // Verify the output
    /*
    for(int i = 0; i < N; ++i){
        for(int j = 0; j < N; ++j){
            for(int k = 0; k < N; ++k){
                ANS[i*N+j] += A[i*N+k] * B[k*N+j];
            }
        }
    }
     */
    bool result = true;
    for(int i = 0; i < elements; i++) {
        if(C[i] != N) {
            result = false;
            break;
        }
    }
    if(result) {
        printf("Output is correct\n");
    } else {
        printf("Output is incorrect\n");
    }
    //-----------------------------------------------------
    // STEP 13: Release OpenCL resources
    //-----------------------------------------------------
    
    // Free OpenCL resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseContext(context);

    // Free host resources
    free(A);
    free(B);
    free(C);
    free(ANS);
    free(platforms);
    free(devices);
}
