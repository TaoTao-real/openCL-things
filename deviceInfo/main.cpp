//
//  main.cpp
//  test
//
//  Created by 李圣韬 on 2020/3/15.
//  Copyright © 2020 李圣韬. All rights reserved.
//
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <OpenCL/OpenCL.h>

void checkerr(cl_int err, int num){
    if(CL_SUCCESS != err){
        printf("OpenCL error(%d) at %d\n", err, num-1);
    }
}

int main(int argc, const char * argv[]) {
    cl_platform_id* platform;
    cl_uint num_platform;
    cl_int err;
    err = clGetPlatformIDs(0, NULL, &num_platform);
    platform = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platform);
    err = clGetPlatformIDs(num_platform, platform, NULL);
    for(int i = 0; i < num_platform; ++i){
        size_t size;
        // get platform name
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, 0, NULL, &size);
        char* Pname = (char*)malloc(size);
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, size, Pname, NULL);
        printf("CL_PLATFORM_NAME: %s\n", Pname);
        // get platform vendor
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_VENDOR, 0, NULL, &size);
        char* PVendor = (char*)malloc(size);
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_VENDOR, size, PVendor, NULL);
        printf("CL_PLATFORM_VENDOR: %s\n", PVendor);
        // get platform version
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_VERSION, 0, NULL, &size);
        char* PVersion = (char*)malloc(size);
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_VERSION, size, PVersion, NULL);
        printf("CL_PLATFORM_VERSION: %s\n", PVersion);
        // get platform profile
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_PROFILE, 0, NULL, &size);
        char* PProfile = (char*)malloc(size);
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_PROFILE, size, PProfile, NULL);
        printf("CL_PLATFORM_PROFILE: %s\n", PProfile);
        // get paltform extensions
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_EXTENSIONS, 0, NULL, &size);
        char* PExten = (char*)malloc(size);
        err = clGetPlatformInfo(platform[i], CL_PLATFORM_EXTENSIONS, size, PExten, NULL);
        printf("CL_PLATFORM_EXTENSIONS: %s\n", PExten);
        
        cl_device_id* devices;
        cl_uint num_device;
        // get the num of device
        err = clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_device);
        devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_device);
        err = clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, num_device, devices, NULL);
        for(int j = 0; j < num_device; ++j){
            // get device name
            char DeviceName[100];
            err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 100, DeviceName, NULL);
            printf("DEVICE_NAME: %s\n", DeviceName);
            // get device opencl version
            char DeviceVersion[100];
            err = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 100, DeviceVersion, NULL);
            printf("DEVICE_VERSION: %s\n", DeviceVersion);
            // get max compute units
            cl_long UnitNum;
            err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &UnitNum, NULL);
            checkerr(err, 73);
            printf("MAX_COMPUTE_UNITS: %ld\n", &UnitNum);
            // get device frequency
            cl_long frequency;
            err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &frequency, NULL);
            checkerr(err, 78);
            printf("MAX_CLOCK_FREQUENCY: %ld(MHz)\n", &frequency);
            // get device global memory size
            cl_long GlobalSize;
            err = clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &GlobalSize, NULL);
            printf("Device Global Size: %0.0f(MB)\n", GlobalSize);
        }
        free(PExten);
        free(PProfile);
        free(PVersion);
        free(PVendor);
        free(Pname);
    }
    free(platform);
    return 0;
}
