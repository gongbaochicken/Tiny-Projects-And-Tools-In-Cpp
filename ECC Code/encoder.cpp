//
//  encoder.cpp
//  myECC
//
//  Created by Jason Zhuo Jia on 3/8/16.
//  Copyright © 2016 Zhuo Jia. All rights reserved.
//

#include "encoder.hpp"
#include "galoisField.hpp"

//Time Complexity: O(kn)
vector<unsigned int> dfs_encoder(int n, vector<unsigned int> m){
    int k = (int) m.size();
    vector<unsigned int> v(n, 1);
    vector<unsigned int> ans(n, 0);
    
    //Calculate column vector
    vector<unsigned int> p(n, 0);
    for(int i = 0; i < k; i++){
        p[i] = m[i];
    }
    
    //Calculate answer in O(nk) time cost
    unsigned int base = 1;
    for(int i = 0; i < n; i++){
        unsigned int sum = 0;
        for(int j = 1; j < k; j++){
            v[j] = multiply28(v[j-1], base);
        }
        for(int j = 0; j < k; j++){
            unsigned int temp = multiply28(v[j], p[j]);
            sum = add28(sum, temp);
        }
        ans[i] = sum;
        base = multiply28(base, 3);
    }
    return ans;
}