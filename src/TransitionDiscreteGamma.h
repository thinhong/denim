//
// Created by thinh on 05/02/2021.
//

#ifndef MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H
#define MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H

#include "TransitionParametricDist.h"

class TransitionDiscreteGamma: public TransitionParametricDist {
private:
    // TODO: change scale to rate 
    double rate {0};
    double shape {0};
    std::vector<double> transitionProb;
public:
    TransitionDiscreteGamma(double rate, double shape);
    // explicit TransitionDiscreteGamma(std::vector<double>& cumulativeProb);
    TransitionDiscreteGamma() = default;
    double getRate();
    double getShape();
};


#endif //MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H
