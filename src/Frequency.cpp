//
// Created by thinh on 22/09/2021.
//

#include "Frequency.h"

Frequency::Frequency(double frequency) {
    this->frequency = frequency;
}

std::string Frequency::getDistName() {
    return "frequency";
}

double Frequency::getTransitionProb(size_t index) {
    return frequency;
}

size_t Frequency::getMaxDay() {
    return 1;
}