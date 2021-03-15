/*
 * @Author: your name
 * @Date: 2020-03-30 22:07:52
 * @LastEditTime: 2020-03-30 22:07:52
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: /undefined/Users/starwish/Desktop/图像旋转李圣韬1901213315/main.cpp
 */
#include "stdafx.h"
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>

#include  "gFreeImage.h"

using namespace std;

#define NWITEMS 4
#pragma comment (lib,"OpenCL.lib")
#pragma comment (lib,"FreeImage.lib")

//把文本文件读入一个string中,其实就是把运行程序传给从机
int convertToString(const char *filename, std::string& s)
{
    size_t size;
    char*  str;

    std::fstream f(filename, (std::fstream::in | std::fstream::binary));

    if(f.is_open())
    {
        size_t fileSize;
        f.seekg(0, std::fstream::end);
        size = fileSize = (size_t)f.tellg();
        f.seekg(0, std::fstream::beg);

        str = new char[size+1];
        if(!str)
        {
            f.close();
            return NULL;
        }

        f.read(str, fileSize);
        f.close();
        str[size] = '\0';

        s = str;
        delete[] str;
        return 0;
    }
    printf("Error: Failed to open file %s\n", filename);
    return 1;
}

//CPU旋转图像：使用CPU来旋转图片
void cpu_rotate(unsigned char* inbuf, unsigned char* outbuf, int w, int h,float sinTheta, float cosTheta)
{
    int i, j;
    int xc = w/2;
    int yc = h/2;

    for(i = 0; i < h; i++)
    {
        for(j=0; j< w; j++)
        {
            int xpos =  ( j-xc)*cosTheta - (i-yc)*sinTheta+xc;    
            int ypos =  (j-xc)*sinTheta + ( i-yc)*cosTheta+yc; 

            if(xpos>=0&&ypos>=0&&xpos<w&&ypos<h)
               outbuf[ypos*w + xpos] = inbuf[i*w+j];
        }
    }
}

int main(int argc, char* argv[])
{
    //装入图像
    unsigned char *src_image=0;
    unsigned char *cpu_image=0;
    int W, H;
    gFreeImage img;
    if(!img.LoadImageGrey("lenna.jpg"))
    {
        printf("装入lenna.jpg失败\n");
        exit(0);
    }
    else
        src_image = img.getImageDataGrey(W, H);

    size_t mem_size = W*H;
    cpu_image = (unsigned char*)malloc(mem_size);

    cl_uint status;
    cl_platform_id platform;

    //创建平台对象
    status = clGetPlatformIDs( 1, &platform, NULL );

    cl_device_id device;

    //创建GPU设备
    clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU,
        1,
        &device,
        NULL);
    //创建context
    cl_context context = clCreateContext( NULL,
        1,
        &device,
        NULL, NULL, NULL);
    //创建命令队列
    cl_command_queue queue = clCreateCommandQueue( context,
        device,
        CL_QUEUE_PROFILING_ENABLE, NULL );

    //创建三个OpenCL内存对象，并把buf1的内容通过隐式拷贝的方式
    //拷贝到clbuf1,buf2的内容通过显示拷贝的方式拷贝到clbuf2
    cl_mem d_ip = clCreateBuffer(
            context, CL_MEM_READ_ONLY,
            mem_size, 
            NULL, NULL);
    cl_mem d_op = clCreateBuffer(
            context, CL_MEM_WRITE_ONLY,
            mem_size, 
            NULL, NULL);
    status = clEnqueueWriteBuffer ( 
        queue , d_ip, CL_TRUE,
        0, mem_size, (void *)src_image,
        0, NULL,  NULL);

    const char * filename  = "kernel.cl";
    std::string  sourceStr;
    status = convertToString(filename, sourceStr);
    const char * source    = sourceStr.c_str();
    size_t sourceSize[]    = { strlen(source) };

    //创建程序对象
    cl_program program = clCreateProgramWithSource(
        context, 
        1, 
        &source,
        sourceSize,
        NULL);

    //编译程序对象
    status = clBuildProgram( program, 1, &device, NULL, NULL, NULL );
    if(status != 0)
    {
        printf("clBuild failed:%d\n", status);
        char tbuf[0x10000];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0x10000, tbuf, NULL);
        printf("\n%s\n", tbuf);
        return -1;
    }


    //创建Kernel对象
    //Use the “image_rotate” function as the kernel 
    //创建Kernel对象
    cl_kernel kernel = clCreateKernel( program, "image_rotate", NULL );

    //设置Kernel参数
    float sintheta = 1, costheta = 0;
    clSetKernelArg(kernel, 0, sizeof(cl_mem),  (void *)&d_ip); 
    clSetKernelArg(kernel, 1, sizeof(cl_mem),  (void *)&d_op);
    clSetKernelArg(kernel, 2, sizeof(cl_int),  (void *)&W);
    clSetKernelArg(kernel, 3, sizeof(cl_int),  (void *)&H);
    clSetKernelArg(kernel, 4, sizeof(cl_float), (void *)&sintheta);
    clSetKernelArg(kernel, 5, sizeof(cl_float), (void *)&costheta);

    //Set local and global workgroup sizes
    size_t localws[2] = {16,16} ; 
    size_t globalws[2] = {W, H};//Assume divisible by 16

    cl_event ev;
    //执行kernel
    clEnqueueNDRangeKernel( 
        queue ,kernel, 
        2, 0, globalws, localws, 
        0, NULL, &ev);

    clFinish( queue );

    //计算kerenl执行时间
    cl_ulong startTime, endTime;
    clGetEventProfilingInfo(ev, CL_PROFILING_COMMAND_START, 
    sizeof(cl_ulong), &startTime, NULL);
    clGetEventProfilingInfo(ev,  CL_PROFILING_COMMAND_END,
    sizeof(cl_ulong), &endTime, NULL);
    cl_ulong kernelExecTimeNs = endTime-startTime;
    printf("kernal exec time :%8.6f ms\n ", kernelExecTimeNs*1e-6 );

    //数据拷回host内存
    // copy results from device back to host
    unsigned char *op_data=0;
    op_data = (cl_uchar *) clEnqueueMapBuffer( queue,
        d_op,
        CL_TRUE,
        CL_MAP_READ,
        0, 
        mem_size,
        0, NULL, NULL, NULL );

    int i;
    cpu_rotate(src_image,cpu_image, W, H, 1, 0);
    for(i = 0; i < mem_size; i++)
    {
        src_image[i] =cpu_image[i];
    }   
    img.SaveImage("cpu_lenna_rotate.jpg");
    for(i = 0; i < mem_size; i++)
    {
        src_image[i] =op_data[i];
    }   
    img.SaveImage("lenna_rotate.jpg");

    if(cpu_image)
        free(cpu_image);

    //删除OpenCL资源对象
    clReleaseMemObject(d_ip); 
    clReleaseMemObject(d_op);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}