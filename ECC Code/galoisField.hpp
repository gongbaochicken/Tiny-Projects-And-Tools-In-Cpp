//
//  galoisField.hpp
//  myECC
//
//  Created by Jason Zhuo Jia on 3/5/16.
//  Copyright © 2016 Zhuo Jia. All rights reserved.
//

#ifndef galoisField_hpp
#define galoisField_hpp

#include <stdio.h>
#include <stdint.h>

unsigned int add28(unsigned int x, unsigned int y);

unsigned int multiply28(unsigned int x, unsigned int y);

unsigned int sub28(unsigned int  x, unsigned int y);

unsigned int inv28(unsigned int x);

#endif /* galoisField_hpp */
