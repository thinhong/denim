//
// Created by thinh on 27/10/2021.
//

#include "myProb.h"
#include "DistributionLogNormal.h"
#include <iostream>

void DistributionLogNormal::calcTransitionProb() {
    // First, generate cumulative probability
    double tempProb {0};
    std::vector<double> cumulativeProb;
    double i {0};
    while (tempProb <= (1 - Distribution::errorTolerance)) {
        tempProb = log_normal_cdf(i, mu, sigma);
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

DistributionLogNormal::DistributionLogNormal(double mu, double sigma) {
    this->mu = mu;
    this->sigma = sigma;
    this->calcTransitionProb();
}

double DistributionLogNormal::getTransitionProb(size_t index) {
    if (index >= transitionProb.size()) {
        return 1;
    } else {
        return transitionProb[index];
    }
}

size_t DistributionLogNormal::getMaxDay() {
    return maxDay;
}

std::string DistributionLogNormal::getDistName() {
    return "lognormal";
}

double DistributionLogNormal::getMu() {
    return mu;
}

double DistributionLogNormal::getSigma() {
    return sigma;
}