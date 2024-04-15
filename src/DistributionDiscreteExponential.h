//
// Created by thinh on 05/03/2021.
//

#ifndef MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
#define MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
#include "DistributionParametric.h"

class DistributionDiscreteExponential: public DistributionParametric {
private:
    double rate {0};
public:
    explicit DistributionDiscreteExponential(double rate);
    double getRate();
};


#endif //MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
