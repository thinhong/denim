//
// Created by anhptq on 22/01/2024.
//

#include <iostream>
#include "myProb.h"
#include "DistributionParametric.h"

double DistributionParametric::getTransitionProb(size_t index) {
    if (index >= transitionProb.size()) {
        return 1;
    } else {
        return this -> transitionProb[index];
    }
}

double DistributionParametric::getProbDist(size_t index) {
    if (index >= probDist.size()) {
        return 0;
    }else{
        return this -> probDist[index];
    }
}

void DistributionParametric::calcTransitionProb(std::function<double(double)> cdf_func) {
    // current cumulative prob
    double tempProb {0};
    // previous cumulative prob
    double prevProb {0};
    double i {0};
    double currTransitionProb {0};
    

    while (true){
        // --- Calculate current cumulative prob 
        // current cumulative prob is value from cdf func if its less than (1 - errorTolerance)
        // otherwise, set it to 1
        tempProb = cdf_func(i) < (1 - Distribution::errorTolerance) ? cdf_func(i) : 1;

        // --- Calculate the current transition prob
        if (i != 0){ // skip fist iteration (i.e. time = 0) 
            // Transition prob = curr prob / (1 - previous cumulative prob) (i.e. gamma_i = p_i / (1 - cdf(i - 1)) )
            currTransitionProb = (tempProb - prevProb) / (1 - prevProb);
            this -> transitionProb.push_back(currTransitionProb);
            this -> probDist.push_back(tempProb - prevProb);
            prevProb = tempProb;
        }

        // --- Update current time
        i += Distribution::timeStep;

        // --- Stop when tempProb, i.e. cumulative probability at 1 
        if (tempProb == 1){
            break;
        }
    }

    // Remember to calculate max day
    this -> maxDay = this -> transitionProb.size();
}

