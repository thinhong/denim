//
// Created by thinh on 27/10/2021.
//

#ifndef COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H
#define COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H

#include "TransitionParametricDist.h"

class TransitionLogNormal: public TransitionParametricDist {
private:
    double mu {0};
    double sigma {0};
public:
    TransitionLogNormal(double mu, double sigma);
    TransitionLogNormal() = default;
};


#endif //COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H
