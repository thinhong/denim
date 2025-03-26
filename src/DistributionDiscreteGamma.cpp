//
// Created by thinh on 05/02/2021.
//

#include "myProb.h"
#include "DistributionDiscreteGamma.h"
#include <iostream>

DistributionDiscreteGamma::DistributionDiscreteGamma(double rate, double shape) {
    // TODO: change scale to rate 
    this->rate = rate;
    this->shape = shape;

    // https://people.sc.fsu.edu/~jburkardt/cpp_src/prob/prob.cpp
    // A controls the location of the peak;  A is often chosen to be 0.0.
    // B is the "scale" parameter; 0.0 < B, and is often 1.0.
    // C is the "shape" parameter; 0.0 < C, and is often 1.0.
    calcTransitionProb(
        [rate, shape](double timestep) {return gamma_cdf(timestep, 0, 1/rate, shape);}
        );
    this->distName = "gamma";
}

// DistributionDiscreteGamma::DistributionDiscreteGamma(std::vector<double> &cumulativeProb) {
//     this->transitionProb = cumulativeProb;
//     this->maxDay = cumulativeProb.size();
//     this->distName = "gamma";
// }

double DistributionDiscreteGamma::getRate() {
    return rate;
}

double DistributionDiscreteGamma::getShape() {
    return shape;
}