//
//  fft_encoder.hpp
//  myECC
//
//  Created by Jason Zhuo Jia on 3/8/16.
//  Copyright © 2016 Zhuo Jia. All rights reserved.
//

#ifndef fft_encoder_hpp
#define fft_encoder_hpp

#include <stdio.h>
#include <vector>

using namespace std;

unsigned int add257(unsigned int a, unsigned int b);

unsigned int sub257(unsigned int a, unsigned int b);

unsigned int mult257(unsigned int a, unsigned int b);

vector<unsigned int> fft_encoder(vector<unsigned int> m);

#endif /* fft_encoder_hpp */
