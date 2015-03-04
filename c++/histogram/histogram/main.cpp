//
//  main.cpp
//  histogram
//
//  Created by Connor Goddard on 03/03/2015.
//  Copyright (c) 2015 Connor Goddard. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

using namespace std;

vector<int> calcHist(float bucketSize, vector<double> values, double maxVal);

int main(int argc, const char * argv[]) {

    const float bucket_size = 10;
    
    //vector<double> values = {0.0102244, 0.028072, 0.0144578, 0.064578, 0.08148, 0.012749};
    vector<double> values = {10, 10, 9, 2, 5, 5, 3, 20};
    
    double maxVal = *max_element(values.begin(), values.end());

    vector<int> hist = calcHist(bucket_size, values, maxVal + 1);
    
    for (std::vector<int>::size_type i = 0; i < hist.size(); ++i) {
        
        cout << (i * bucket_size) << "-" << ((i + 1) * bucket_size - 1) << " -> " << hist[i] << "\n";
        
    }
    
    return 0;
    
}

vector<int> calcHist(float bucketSize, vector<double> values, double maxVal) {
    
    int number_of_buckets = (int)ceil(maxVal / bucketSize);
    
    vector<int> histogram(number_of_buckets);
    
    for(vector<double>::const_iterator it = values.begin(); it != values.end(); ++it) {
        
        int bucket = (int)floor(*it / bucketSize);
        histogram[bucket] += 1;
        
    }
    
    return histogram;

}
