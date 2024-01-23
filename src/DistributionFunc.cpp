//
// Created by anhptq on 22/01/2024.
//

#include <iostream>
#include "myProb.h"
#include "DistributionFunc.h"

double DistributionFunc::getTransitionProb(size_t index) {
    if (index >= transitionProb.size()) {
        return 1;
    } else {
        return transitionProb[index];
    }
}

void DistributionFunc::calcTransitionProb(std::function<double(int)> cdf_func) {
    // First, generate cumulative probability
    double tempProb {0};
    std::vector<double> cumulativeProb;
    double i {0};
    while (tempProb < (1 - Distribution::errorTolerance)) {
        tempProb = cdf_func(i);
        cumulativeProb.push_back(tempProb);
        i += Distribution::timeStep;
    }
    cumulativeProb.push_back(1);

    // Then compute P(0 < waiting time <= 1) by cdf(1) - cdf(0)
    std::vector<double> waitingTime;
    // Vector storing cumulative prob

    for (size_t j {0}; j < (cumulativeProb.size() - 1); ++j) {
        tempProb = cumulativeProb[j + 1] - cumulativeProb[j];
        waitingTime.push_back(tempProb);

        // calculate current transition prob
        double currTransitionProb = 0;
        if (j == 0){
            currTransitionProb = tempProb;
        }else{
            currTransitionProb = tempProb/(1 - cumulativeProb[j]);
        }

        transitionProb.push_back(currTransitionProb);
    }

    // Remember to calculate max day
    this -> maxDay = transitionProb.size();
//    std::cout << maxDay << "\n";
}

