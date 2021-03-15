//
//  normalVectorAdd.cpp
//  矢量加法
//
//  Created by 李圣韬 on 2020/3/16.
//  Copyright © 2020 李圣韬. All rights reserved.
//

#include "normalVectorAdd.hpp"

void normalVectorAdd(int n){
    int* A, *B, *C;
    int elementnum = n;
    A = (int*)malloc(sizeof(int) * elementnum);
    B = (int*)malloc(sizeof(int) * elementnum);
    C = (int*)malloc(sizeof(int) * elementnum);
    for(int i = 0; i < elementnum; ++i){
        A[i] = i;
        B[i] = i;
    }
    clock_t begin = clock();
    for(int i = 0; i < elementnum; ++i){
        C[i] = A[i] + B[i];
    }
    clock_t end = clock();
    
    // Verify the output
    bool result = true;
    for(int i = 0; i < elementnum; i++) {
        if(C[i] != i+i) {
            result = false;
            break;
        }
    }
    if(result) {
        printf("Output is correct\n");
    } else {
        printf("Output is incorrect\n");
    }
    
    cout << "CPU串行向量加法用时： "<< end - begin << "click" << endl;
    free(A);
    free(B);
    free(C);
}
