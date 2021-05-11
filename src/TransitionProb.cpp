//
// Created by thinh on 08/02/2021.
//

#include "TransitionProb.h"

TransitionProb::TransitionProb(double transitionProb) {
    this->transitionProb = transitionProb;
}

double TransitionProb::getTransitionProb(size_t index) {
    return transitionProb;
}

size_t TransitionProb::getMaxDay() {
    return 1;
}

std::string TransitionProb::getDistName() {
    return distName;
}