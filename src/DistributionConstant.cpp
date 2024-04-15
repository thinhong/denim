//
// Created by thinh on 22/09/2021.
//

#include "DistributionConstant.h"

DistributionConstant::DistributionConstant(double constValue) {
    this->constant = constValue;

    // update dist name and max date
    this->distName = "constant";
    this->maxDay = 1;
}

double DistributionConstant::getTransitionProb(size_t index) {
    return constant;
}
