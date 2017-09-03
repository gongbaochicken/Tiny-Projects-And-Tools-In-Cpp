//
//  decoder.hpp
//  myECC
//
//  Created by Jason Zhuo Jia on 3/9/16.
//  Copyright © 2016 Zhuo Jia. All rights reserved.
//

#ifndef decoder_hpp
#define decoder_hpp

#include <stdio.h>
#include <vector>
using namespace std;

//vector<double> gauss(vector< vector<double> > M);
vector<unsigned int> gaussElimination(vector<vector<unsigned int>> M);

//Using GF(2^8) to decode the codeword
vector<unsigned int> decoder(int k, int n, vector<unsigned int> c);

#endif /* decoder_hpp */
