//
// Created by thinh on 04/03/2021.
//

#include <algorithm>
#include <stdexcept>
#include "DistributionNonparametric.h"

DistributionNonparametric::DistributionNonparametric(std::vector<double> waitingTime) {
    // Make sure that waiting time distribution is a probability distribution (sum = 1)
    double sumWaitingTime {0};
    for (auto& wt: waitingTime) {
        sumWaitingTime += wt;
    }
    if (sumWaitingTime != 1) {
        for (size_t i {0}; i < waitingTime.size(); ++i) {
            waitingTime[i] /= sumWaitingTime;
        }
    }
    this->waitingTime = waitingTime;
    this->calcTransitionProb();
    this->distName = "nonparametric";
}

void DistributionNonparametric::calcTransitionProb() { 
    // variable to keep track of cumulated prob
    // at iteration i, cumulatedProb = p1 + p2 + ... + p[i-1]
    double cumulatedProb {0};

    // --- Compute transitionProb
    // x1 = p1 / (1 - p0), x2 = p2 / (1 - (p0 + p1)), x3 = p3 / (1 - (p0 + p1 + p2)) and so on
    for (auto &currProb: waitingTime) {
        this -> transitionProb.push_back( (currProb/(1 - cumulatedProb)) );
        cumulatedProb += currProb;
    }

    // Remember to calculate max day
    this -> maxDay = this -> transitionProb.size();
}

double DistributionNonparametric::getTransitionProb(size_t index) {
    if (index >= transitionProb.size()) {
        return 1;
    } else {
        return transitionProb[index];
    }
}


std::vector<double> DistributionNonparametric::getWaitingTime() {
    return waitingTime;
}