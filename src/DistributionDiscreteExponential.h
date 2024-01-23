//
// Created by thinh on 05/03/2021.
//

#ifndef MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
#define MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
#include "DistributionFunc.h"

class DistributionDiscreteExponential: public DistributionFunc {
private:
    double rate {0};
public:
    explicit DistributionDiscreteExponential(double rate);
    double getRate();
};


#endif //MAIN_CPP_DISCRETEEXPONENTIALDISTRIBUTION_H
