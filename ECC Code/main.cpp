//
//  main.cpp
//  myECC
//
//  Created by Jason Zhuo Jia on 3/5/16.
//  Copyright © 2016 Zhuo Jia. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "galoisField.hpp"
#include "encoder.hpp"
#include "fft_encoder.hpp"
#include "decoder.hpp"

using namespace std;

//Test function:
void printMatrix(vector<vector<unsigned int> > m){
    cout << "######################" << endl;
    for(int i = 0; i < (int)m.size(); i++){
      for (int j = 0; j < (int)m[0].size(); j++) {
            cout << dec<< m[i][j] <<" ";
        }
        cout << endl;
    }
    cout << endl;
}

void printVector(vector<unsigned int> v){
    cout << "######################" << endl;
    for(int i = 0; i < (int)v.size();i++){
        cout << v[i] << " ";
    }
    cout<<endl;
    cout << endl;
}

int main(int argc, const char * argv[]) {
    cout<< "Let's test case#1: m = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}"<< endl;
    vector<unsigned int> m1 = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    
    cout << "######################" << endl;
    cout<<"info encode using DFT:"<<endl;
    vector<unsigned int> ans1 = dfs_encoder(20, m1);
    for(int i = 0; i < (int)ans1.size(); i++){
        cout<< int(ans1[i])<<" ";
    }
    cout<<endl;
    cout << "######################" << endl;
    cout<<"info encode using FFT:"<<endl;
    vector<unsigned int> ans1_fft = fft_encoder(m1);
    for(int i = 0; i < (int)ans1_fft.size(); i++){
        cout<< int(ans1_fft[i])<<" ";
    }
    cout << endl;
    
    cout<< "decode with no missing elements:" <<endl;
    vector<unsigned int> codeword1 = ans1;
    vector<unsigned int> result1 = decoder(10, 20, codeword1);
    printVector(result1);
    cout << endl;
    cout<< "decode with some missing elements:" <<endl;
    //290, 300 are indicators as missing, since they are out of G(2^8)
    vector<unsigned int> codeword0 = {{0, 180, 253, 290, 300, 236, 165, 202, 14, 244, 13, 93, 231, 54, 117, 97, 84, 225, 86, 51}};
    vector<unsigned int> result10 = decoder(10 ,20, codeword0);
    printVector(result10);
    
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    
    cout<< "Let's test case#2: m = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67}"<< endl;
    vector<unsigned int> m2 = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67}};
    vector<unsigned int> ans2 = dfs_encoder(100, m2);
    
    cout << "######################" << endl;
    cout<<"info encode using DFT:"<<endl;
    for(int i = 0; i < (int)ans2.size(); i++){
        cout<< int(ans2[i])<<" ";
    }
    cout << endl;
    cout << endl;
    cout<<"info encode using FFT:"<<endl;
    vector<unsigned int> ans2_fft = fft_encoder(m2);
    for(int i = 0; i < (int)ans2_fft.size(); i++){
        cout<< int(ans2_fft[i])<<" ";
    }
    cout<<endl;
    cout << "######################" << endl;
    cout<< "decode with no missing elements:" <<endl;
    vector<unsigned int> codeword2 = ans2;
    vector<unsigned int> result2 = decoder(26, 100, codeword2);
    printVector(result2);
    
    cout<< "decode with some missing elements:" <<endl;
    //367, 330 are indicators as missing, since they are out of G(2^8)
    vector<unsigned int> codeword2_0 = {{79, 226, 330, 211, 367, 142, 110, 49, 178, 93, 233, 94, 8, 135, 81, 196, 212, 33, 109, 63, 38, 226, 217, 19, 108, 89, 96, 27, 51, 207, 198, 215, 250, 82, 105, 189, 231, 185, 202, 186, 196, 128, 232, 201, 33, 102, 227, 201, 96, 180, 101, 246, 53, 118, 138, 239, 143, 252, 131, 121, 242, 180, 91, 234, 148, 3, 191, 95, 133, 4, 135, 90, 176, 103, 135, 226, 134, 226, 3, 8, 68, 173, 1, 4, 221, 154, 160, 185, 231, 44, 83, 250, 239, 187, 48, 140, 24, 19, 198, 157}};
    vector<unsigned int> result2_0 = decoder(26, 100, codeword2_0);
    printVector(result2_0);
    
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    
    cout<< "Let's test case#3: m = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67}"<< endl;
    vector<unsigned int> m3 = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67, 1, 1, 34, 45, 67, 23, 89, 53, 12, 54, 67}};
    
    cout<<"info encode using DFT:"<<endl;
    vector<unsigned int> ans3 = dfs_encoder(255, m3);
    for(int i = 0; i < (int)ans3.size(); i++){
        cout<< int(ans3[i])<<" ";
    }
    cout << endl;
    
    cout<< "decode with no missing elements:" <<endl;
    vector<unsigned int> codeword3 = ans3;
    vector<unsigned int> result3 = decoder(69, 255, codeword3);
    printVector(result3);
    cout << endl;
    cout<< "decode with some missing elements:" <<endl;
    //290, 300 are indicators as missing, since they are out of G(2^8)
    vector<unsigned int> codeword0_3 = {{290, 300, 14, 230, 141, 250, 69, 97, 115, 157, 70, 116, 71, 50, 228, 223, 46, 101, 222, 246, 185, 166, 145, 121, 0, 224, 33, 84, 94, 115, 5, 126, 167, 223, 226, 67, 99, 54, 177, 213, 79, 33, 203, 76, 72, 65, 61, 113, 60, 118, 139, 176, 183, 255, 167, 212, 197, 62, 195, 182, 4, 107, 219, 186, 193, 94, 143, 181, 170, 235, 60, 26, 101, 233, 181, 94, 82, 11, 12, 121, 141, 56, 48, 220, 254, 38, 78, 30, 168, 81, 115, 158, 239, 214, 29, 26, 92, 176, 8, 225, 50, 228, 237, 255, 15, 100, 171, 96, 76, 200, 80, 206, 48, 125, 215, 212, 34, 20, 25, 61, 125, 171, 50, 62, 201, 137, 36, 176, 78, 76, 97, 187, 23, 50, 121, 66, 55, 81, 145, 168, 58, 3, 107, 135, 14, 237, 252, 19, 241, 17, 129, 28, 108, 80, 113, 202, 173, 179, 35, 199, 236, 103, 206, 148, 109, 10, 22, 229, 11, 159, 42, 33, 196, 51, 93, 184, 58, 87, 151, 237, 89, 84, 158, 174, 181, 119, 71, 228, 91, 174, 128, 235, 223, 251, 253, 219, 26, 161, 72, 254, 133, 134, 149, 96, 12, 236, 95, 171, 25, 63, 226, 177, 45, 101, 39, 68, 117, 5, 223, 45, 48, 93, 97, 173, 46, 154, 62, 199, 75, 91, 50, 140, 60, 110, 194, 66, 77, 196, 222, 228, 204, 144, 217, 34, 123, 159, 27, 67, 0, 215, 5, 42, 201, 150, 163}};
    vector<unsigned int> result10_3 = decoder(69, 255, codeword0_3);
    printVector(result10_3);

    return 0;
}
