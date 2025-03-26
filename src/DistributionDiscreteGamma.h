//
// Created by thinh on 05/02/2021.
//

#ifndef MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H
#define MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H

#include "DistributionParametric.h"

class DistributionDiscreteGamma: public DistributionParametric {
private:
    // TODO: change scale to rate 
    double rate {0};
    double shape {0};
    std::vector<double> transitionProb;
public:
    DistributionDiscreteGamma(double rate, double shape);
    // explicit DistributionDiscreteGamma(std::vector<double>& cumulativeProb);
    DistributionDiscreteGamma() = default;
    double getRate();
    double getShape();
};


#endif //MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H
