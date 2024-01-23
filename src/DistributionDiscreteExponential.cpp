//
// Created by thinh on 05/03/2021.
//

#include <iostream>
#include "DistributionDiscreteExponential.h"
#include "myProb.h"


DistributionDiscreteExponential::DistributionDiscreteExponential(double rate) {
    this->rate = rate;
    // call calcTransitionProb inherited from DistributionFunc 
    // pass exponential_cdf function with its parameter
    // https://people.sc.fsu.edu/~jburkardt/cpp_src/prob/prob.cpp
    calcTransitionProb([rate](int i){return exponential_cdf(i, 0, 1/rate);});
    this->distName = "exponential";
}


double DistributionDiscreteExponential::getRate() {
    return rate;
}