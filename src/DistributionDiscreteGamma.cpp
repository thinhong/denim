//
// Created by thinh on 05/02/2021.
//

#include "myProb.h"
#include "DistributionDiscreteGamma.h"
#include <iostream>

void DistributionDiscreteGamma::calcTransitionProb() {
    // First, generate cumulative probability
    double tempProb {0};
    std::vector<double> cumulativeProb;
    double i {0};
    while (tempProb <= (1 - Distribution::errorTolerance)) {
        // https://people.sc.fsu.edu/~jburkardt/cpp_src/prob/prob.cpp
        // A controls the location of the peak;  A is often chosen to be 0.0.
        // B is the "scale" parameter; 0.0 < B, and is often 1.0.
        // C is the "shape" parameter; 0.0 < C, and is often 1.0.
        tempProb = gamma_cdf(i, 0, scale, shape);
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
//    std::cout << maxDay << "\n";
}

DistributionDiscreteGamma::DistributionDiscreteGamma(double scale, double shape) {
    this->scale = scale;
    this->shape = shape;
    this->calcTransitionProb();
}

DistributionDiscreteGamma::DistributionDiscreteGamma(std::vector<double> &cumulativeProb) {
    this->transitionProb = cumulativeProb;
    this->maxDay = cumulativeProb.size();
}

double DistributionDiscreteGamma::getTransitionProb(size_t index) {
    if (index >= transitionProb.size()) {
        return 1;
    } else {
        return transitionProb[index];
    }
}

size_t DistributionDiscreteGamma::getMaxDay() {
    return maxDay;
}

std::string DistributionDiscreteGamma::getDistName() {
    return "gamma";
}

double DistributionDiscreteGamma::getScale() {
    return scale;
}

double DistributionDiscreteGamma::getShape() {
    return shape;
}