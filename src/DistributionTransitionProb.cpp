//
// Created by thinh on 08/02/2021.
//

#include "DistributionTransitionProb.h"

DistributionTransitionProb::DistributionTransitionProb(double transitionProb) {
    this->transitionProb = transitionProb;
}

double DistributionTransitionProb::getTransitionProb(size_t index) {
    return transitionProb;
}

size_t DistributionTransitionProb::getMaxDay() {
    return 1;
}

std::string DistributionTransitionProb::getDistName() {
    return "transitionProb";
}