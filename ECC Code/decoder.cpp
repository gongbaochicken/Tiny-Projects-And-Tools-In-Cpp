//
//  decoder.cpp
//  myECC
//
//  Created by Jason Zhuo Jia on 3/9/16.
//  Copyright © 2016 Zhuo Jia. All rights reserved.
//

#include "decoder.hpp"
#include "galoisField.hpp"
#include <vector>
#include <iostream>
#include <cmath>
using namespace std;

//Gaussian Elimination
vector<unsigned int> gaussElimination(vector<vector<unsigned int> > M){
    int n = (int) M.size();
    for (int i = 0; i < n; i++) {
        // Search for max in column
        unsigned int max = M[i][i];
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if(M[k][i] > max) {
                max = M[k][i];
                maxRow = k;
            }
        }
        
        //Swap maximum row
        for (int k = i; k < n+1; k++) {
            unsigned int tmp = M[maxRow][k];
            M[maxRow][k] = M[i][k];
            M[i][k] = tmp;
        }
        
        //eliminate below as 0
        for (int k = i+1; k < n; k++) {
            unsigned int c = multiply28(M[k][i], inv28(M[i][i]));
            for (int j = i; j < n+1; j++) {
                if (i == j) {
                    M[k][j] = 0;
                } else {
                    M[k][j] = sub28(M[k][j], multiply28(c, M[i][j]));
                }
            }
        }
    }
    
    //transform to get an upper triangular matrix while solve the linear equation
    vector<unsigned int> x(n);
    for (int i = n-1; i >= 0; i--) {
        x[i] = multiply28(M[i][n], inv28(M[i][i]));
        for (int k = i-1; k >= 0; k--) {
            M[k][n] = sub28(M[k][n], multiply28(x[i], M[k][i]));
        }
    }
    return x;
}

//Using GF(2^8) to decode the codeword, generator = 3
vector<unsigned int> decoder(int k, int n, vector<unsigned int> c){
    int size = (int)c.size();
    vector<unsigned int> ans;
    
    //built the original matrix
    vector<vector<unsigned int> > matrix(size, vector<unsigned>(size, 1));
    unsigned int base = 1;
    for(int i = 0; i < size; i++){
        for(int j = 1; j < size; j++){
            matrix[i][j] = multiply28(matrix[i][j-1], base);
        }
        base = multiply28(base, 3);
    }
    
    //find the missing elements and build K*(K+1) matrix
    vector<vector<unsigned int> > newMatrix;
    int lineCount = 0;
    for(int i = 0; i < size; i++){
        if(lineCount == k) break;
        if(c[i] > 256){  //missing
            continue;
        }else{
            vector<unsigned int> v;
            for(int h = 0; h < k; h++){
                v.push_back(matrix[i][h]);
            }
            v.push_back(c[i]);
            newMatrix.push_back(v);
            lineCount++;
        }
    }
    
    ans = gaussElimination(newMatrix);
    ans.resize(k);
    
    return ans;
}
