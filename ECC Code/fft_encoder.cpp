//
//  fft_encoder.cpp
//  myECC
//
//  Created by Jason Zhuo Jia on 3/8/16.
//  Copyright © 2016 Zhuo Jia. All rights reserved.
//

#include "fft_encoder.hpp"
#include <vector>
#include <complex>
using namespace std;

unsigned int add257(unsigned int a, unsigned int b) {
    return (a + b) % 257;
}

unsigned int sub257(unsigned int a, unsigned int b) {
    int c = a - b;
    if(c >= 0) return c;
    return (unsigned int) (257 + c) ;
}

unsigned int mult257(unsigned int a, unsigned int b) {
    unsigned int c  = (a * b) % 257;
    return c;
}

int findW(int n){
    int p = 256/n;
    return (int)pow(3, p) % 257;
}

vector<unsigned int> recursive_FFT(vector<unsigned int> a){
    int n = (int) a.size();
    if(n == 1) return a;
    vector<unsigned int> a0(n/2, 0);
    vector<unsigned int> a1(n/2, 0);
    //int wn = findW(n), w = 1;
    int wn = findW(n), w = 1;
    for(int i = 0; i < (int)a.size(); i++) {
        if(i % 2 == 0) a0[i/2] = a[i];
        else a1[i/2] = a[i];
    }
    vector<unsigned int> y0 = recursive_FFT(a0);
    vector<unsigned int> y1 = recursive_FFT(a1);
    vector<unsigned int> y(n, 0);
    for(int k = 0; k < n/2; k++){
        y[k] = add257(y0[k],  mult257(w, y1[k])); //y[k] = y0[k] + w*y1[k];
        y[k+n/2] = sub257(y0[k], mult257(w, y1[k]));
        w = mult257(w, wn);
    }
    return y;
}

vector<unsigned int> fft_encoder(vector<unsigned int> m){
    int size = 256;
    vector<unsigned int> ans;
    vector<unsigned int> a(size, 0);

    for(int i = 0; i < (int)m.size(); i++) a[i] = m[i];
    ans = recursive_FFT(a);

    return ans;
}
