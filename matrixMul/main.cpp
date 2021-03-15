//
//  main.cpp
//  matrixMul
//
//  Created by 李圣韬 on 2020/3/22.
//  Copyright © 2020 李圣韬. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include "openclMatrixMul.hpp"
#include "openclMatrixMulPro.hpp"
using namespace std;

float* A = NULL;
float* B = NULL;
float* C = NULL;
float* ANS = NULL;

int main(int argc, const char * argv[]) {
    
    //int n = 4196;
    int n = 16;
    for(int i = 1; i<=100; ++i){
        openclMatrixMul(n*i);
        openclMatrixMulPro(n*i);
    }
    return 0;
}
