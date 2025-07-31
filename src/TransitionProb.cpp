//
// Created by thinh on 08/02/2021.
//

#include "TransitionProb.h"

TransitionProb::TransitionProb(double transitionProb) {
    this->transitionProb = transitionProb;
    this->maxDay = 1;
    this->distName = "transitionProb";
}

double TransitionProb::getTransitionProb(size_t index) {
    return transitionProb;
}

std::vector<double>&  TransitionProb::getTransitionProbRef() {
    static std::vector<double> returned_val{transitionProb};
    return returned_val;
}
