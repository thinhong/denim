//
// Created by thinh on 22/09/2021.
//

#include "TransitionConstant.h"

TransitionConstant::TransitionConstant(double constValue) {
    this->constant = constValue;

    // update dist name and max date
    this->distName = "constant";
    this->maxDay = 1;
}

double TransitionConstant::getTransitionProb(size_t index) {
    return constant;
}

std::vector<double>& TransitionConstant::getTransitionProbRef() {
    static std::vector<double> returned_val{constant};
    return returned_val;
}
