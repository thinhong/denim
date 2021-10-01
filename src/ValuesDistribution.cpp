//
// Created by thinh on 04/03/2021.
//

#include <algorithm>
#include <stdexcept>
#include "ValuesDistribution.h"

void ValuesDistribution::calcTransitionProb() {
    // Compute transitionProb using waiting time
    for (size_t k {0}; k < waitingTime.size(); ++k) {
        transitionProb.push_back(calcTransitionProbHelper(waitingTime, k));
    }

    // Remember to calculate max day
    maxDay = transitionProb.size();
}

ValuesDistribution::ValuesDistribution(std::vector<double> waitingTime) {
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
}

std::string ValuesDistribution::getDistName() {
    return distName;
}

double ValuesDistribution::getTransitionProb(size_t index) {
    if (index >= transitionProb.size()) {
        return 1;
    } else {
        return transitionProb[index];
    }
}

size_t ValuesDistribution::getMaxDay() {
    return maxDay;
}

std::vector<double> ValuesDistribution::getWaitingTime() {
    return waitingTime;
}