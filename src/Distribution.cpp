//
// Created by thinh on 05/02/2021.
//

#include "Distribution.h"

double Distribution::calcTransitionProbHelper(std::vector<double> waitingTime, size_t i) {
    double prob_i {0};
    // Remember that C++ use 0-based indexing
    // x0 = p0
    if (i == 0) {
        prob_i = waitingTime[i];
        // x1 = p1 / (1 - p0), x2 = p2 / (1 - (p0 + p1)), x3 = p3 / (1 - (p0 + p1 + p2)) and so on
    } else {
        double denominator {0};
        for (size_t j {0}; j < i; ++j) {
            denominator += waitingTime[j];
        }
        prob_i = waitingTime[i] / (1 - denominator);
    }
    return prob_i;
}

Distribution::~Distribution() {}