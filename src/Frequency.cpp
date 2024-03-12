//
// Created by thinh on 22/09/2021.
//

#include "Frequency.h"

Frequency::Frequency(double frequency) {
    this->frequency = frequency;
    this->distName = "frequency";
    this->maxDay = 1;
}


double Frequency::getTransitionProb(size_t index) {
    return frequency;
}
