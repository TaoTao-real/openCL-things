//
//  main.cpp
//  异构计算第七周作业
//
//  Created by 李圣韬 on 2020/4/13.
//  Copyright © 2020 李圣韬. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <OpenCL/OpenCL.h>
#include "scalarConvolutionalCompute.hpp"
using namespace std;

const int MAXN = 2000*2000;
const int MAXM = 4*4;
float matrix[MAXN] = {0};
float filter[MAXN] = {0};
int m, n;

int main(int argc, const char * argv[]) {
    cin >> m >> n;
    for(int i = 0; i<4; ++i){
        for(int j = 0; j<4;++j){
            srand((unsigned)time(NULL));
            filter[i*4+j]= rand()%10;
        }
    }
    for(int i = 1; i<=m; ++i){
        for(int j = 1; j<=n; ++j){
            cin >> matrix[i*m+j];
        }
    }
    float* result1 = NULL, * result2 = NULL;
    result1 = scalarConvolutionalCompute(m, n, matrix, 4, 4, filter);
    result2 = openclConclution(matrix);
    return 0;
}
