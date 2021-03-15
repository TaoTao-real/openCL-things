//
//  scalarConvolutionalCompute.cpp
//  异构计算第七周作业
//
//  Created by 李圣韬 on 2020/4/13.
//  Copyright © 2020 李圣韬. All rights reserved.
//

#include "scalarConvolutionalCompute.hpp"
using namespace std;

float* scalarConvolutionalCompute(const int matrixH, const int matrixW, const float* matrix, const int filterH, const int filterW, const float* filter){
    // 保证卷积生成的矩阵和原始矩阵的长宽相同
    float* result = (float*)malloc(sizeof(float)*matrixH*matrixW);
    clock_t start, end;
    start = clock();
    for(int i = 1; i <= matrixH; ++i){
        for(int j = 1; j <= matrixW; ++j){
            int sum = 0;
            for(int y = 0; y < filterH; ++y){
                for(int x = 0; x < filterW;++x){
                    sum += matrix[(i+y)*matrixW + j + x] * filter[y* filterW + x];
                }
            }
            result[i*matrixW+j] = sum;
        }
    }
    end = clock();
    cout << "scalr vonvolution compte cost " << (double)(end - start)/CLOCKS_PER_SEC << endl;
    return result;
}
