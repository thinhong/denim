//
// Created by thinh on 08/02/2021.
//

#include "DistributionTransitionProb.h"

DistributionTransitionProb::DistributionTransitionProb(double transitionProb) {
    this->transitionProb = transitionProb;
    this->maxDay = 1;
    this->distName = "transitionProb";
}

double DistributionTransitionProb::getTransitionProb(size_t index) {
    return transitionProb;
}
