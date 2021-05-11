//
// Created by thinh on 05/03/2021.
//

#ifndef MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
#define MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H

#include "Distribution.h"

class DiscreteExponentialDistribution: public Distribution {
private:
    std::string distName {"exponential"};
    double rate {0};
    size_t maxDay {0};
    std::vector<double> transitionProb;
    void calcTransitionProb();
public:
    explicit DiscreteExponentialDistribution(double rate);
    std::string getDistName() override;
    double getTransitionProb(size_t index) override;
    size_t getMaxDay() override;
    double getRate();
};


#endif //MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
