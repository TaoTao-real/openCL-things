//
//  main.cpp
//  矢量加法
//
//  Created by 李圣韬 on 2020/3/16.
//  Copyright © 2020 李圣韬. All rights reserved.
//

#include <iostream>
#include <time.h>
#include "normalVectorAdd.hpp"
#include "openclVectorAdd.hpp"
#include "openclVectorAddOnlyCPU.hpp"
using namespace std;

const int elementnum = 800000;

int main(int argc, const char * argv[]) {
    
    // normal vector add function
    normalVectorAdd(elementnum);
    
    // opencl vector add function
    openclVectorAdd(elementnum);
    
    // opencl vector add function only CPU
    openclVectorAddOnlyCPU(elementnum);
    
    return 0;
}
