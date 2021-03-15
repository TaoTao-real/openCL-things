//
//  openclConvolutionalCompute.cpp
//  异构计算第七周作业
//
//  Created by 李圣韬 on 2020/4/13.
//  Copyright © 2020 李圣韬. All rights reserved.
//

#include "openclConvolutionalCompute.hpp"

using namespace std;

float* openclConvolutionalCompute(const int& matrixH,const int& matrixW, const float* matrix, int filterH, int filterW, const float* filter){
    /*=================================================
    define parameters
    =================================================*/
    cl_platform_id    platform_id = NULL;
    cl_uint            ret_num_platforms;
    cl_device_id    device_id = NULL;
    cl_uint            ret_num_devices;
    cl_context        context = NULL;
    cl_command_queue command_queue = NULL;
    cl_mem            data_in = NULL;
    cl_mem            data_out = NULL;
    cl_mem            filter_in = NULL;
    cl_program        program = NULL;
    cl_kernel        kernel = NULL;
    size_t            kernel_code_size;
    char            *kernel_str;
    float               *result;
    cl_int            ret;
    FILE            *fp;
    cl_uint            work_dim;
    size_t            global_item_size[2];
    size_t            local_item_size[2];

    /*=================================================
    load kernel, opencl environment setup
    create input and output buffer
    set kernel arguments, excute kernels
    get final results
    =================================================*/
    result = (float*)malloc(matrixW * matrixH * sizeof(float));

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
        &ret_num_devices);
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);
    fp = fopen("Conv2D.cl", "r");
    fseek(fp, 0, SEEK_END);
    long kernel_str_len = ftell(fp);
    rewind(fp);
    kernel_str = (char*)malloc(sizeof(char)*kernel_str_len);
    kernel_str[kernel_str_len] = '\0';
    fread(kernel_str, sizeof(char), kernel_str_len, fp);
    fclose(fp);
    
    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_str,
        (const size_t *)&kernel_code_size, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "Conv2D", &ret);
    data_in = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixH * matrixW * sizeof(float), NULL, &ret);
    data_out = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixH* matrixW * sizeof(float), NULL, &ret);
    filter_in = clCreateBuffer(context, CL_MEM_READ_WRITE, filterH*filterW * sizeof(float), NULL, &ret);

    //write image data into data_in buffer
    ret = clEnqueueWriteBuffer(command_queue, data_in, CL_TRUE, 0, matrixH*matrixW * sizeof(int), matrix, 0, NULL, NULL);

    //write filter data into filter_in buffer
    ret = clEnqueueWriteBuffer(command_queue, filter_in, CL_TRUE, 0, filterW*filterW * sizeof(int), filter, 0, NULL, NULL);

    //set kernel arguments
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&data_in);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&filter_in);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&filterW);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&data_out);

    work_dim = 2;
    
    global_item_size[0] = matrixH;
    global_item_size[1] = matrixW;
    local_item_size[0] = 1;
    local_item_size[1] = 1;

    //execute data parallel kernel */
    ret = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, NULL,
        global_item_size, local_item_size, 0, NULL, NULL);


    // read data_out to host
    ret = clEnqueueReadBuffer(command_queue, data_out, CL_TRUE, 0,
        matrixH*matrixW * sizeof(int), result, 0, NULL, NULL);

    /*=================================================
    release all opencl objects
    =================================================*/
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(data_in);
    ret = clReleaseMemObject(data_out);
    ret = clReleaseMemObject(filter_in);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(result);
    free(kernel_str);
    return result;
}
