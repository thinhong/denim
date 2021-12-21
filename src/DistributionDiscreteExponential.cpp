//
// Created by thinh on 05/03/2021.
//

#include <iostream>
#include "DistributionDiscreteExponential.h"
#include "myProb.h"

void DistributionDiscreteExponential::calcTransitionProb() {
    // First, generate cumulative probability
    double tempProb {0};
    std::vector<double> cumulativeProb;
    double i {0};
    while (tempProb <= (1 - Distribution::errorTolerance)) {
        // https://people.sc.fsu.edu/~jburkardt/cpp_src/prob/prob.cpp
        tempProb = exponential_cdf(i, 0, 1/rate);
        cumulativeProb.push_back(tempProb);
        i += Distribution::timeStep;
    }
    cumulativeProb.push_back(1);

    // Then compute P(0 < waiting time <= 1) by cdf(1) - cdf(0)
    std::vector<double> waitingTime;
    for (size_t j {0}; j < (cumulativeProb.size() - 1); ++j) {
        tempProb = cumulativeProb[j + 1] - cumulativeProb[j];
        waitingTime.push_back(tempProb);
    }

    // Finally, compute transitionProb using waiting time
    for (size_t k {0}; k < waitingTime.size(); ++k) {
        transitionProb.push_back(calcTransitionProbHelper(waitingTime, k));
    }

    // Remember to calculate max day
    maxDay = transitionProb.size();
}

DistributionDiscreteExponential::DistributionDiscreteExponential(double rate) {
    this->rate = rate;
    this->calcTransitionProb();
}

std::string DistributionDiscreteExponential::getDistName() {
    return "exponential";
}

double DistributionDiscreteExponential::getTransitionProb(size_t index) {
    if (index >= transitionProb.size()) {
        return 1;
    } else {
        return transitionProb[index];
    }
}

size_t DistributionDiscreteExponential::getMaxDay() {
    return maxDay;
}

double DistributionDiscreteExponential::getRate() {
    return rate;
}