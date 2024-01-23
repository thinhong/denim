//
// Created by thinh on 08/02/2021.
//

#include <iostream>
#include "myProb.h"
#include "DistributionDiscreteWeibull.h"

DistributionDiscreteWeibull::DistributionDiscreteWeibull(double scale, double shape) {
    this->scale = scale;
    this->shape = shape;
    // https://people.sc.fsu.edu/~jburkardt/cpp_src/prob/prob.cpp
        // A controls the location of the peak;  A is often chosen to be 0.0.
        // B is the "scale" parameter; 0.0 < B, and is often 1.0.
        // C is the "shape" parameter; 0.0 < C, and is often 1.0.
    calcTransitionProb(
        [scale, shape](int i){ return weibull_cdf(i, 0, scale, shape); }
        );
    this->distName = "weibull";
}

double DistributionDiscreteWeibull::getScale() {
    return scale;
}

double DistributionDiscreteWeibull::getShape() {
    return shape;
}
