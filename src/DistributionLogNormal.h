//
// Created by thinh on 27/10/2021.
//

#ifndef COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H
#define COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H

#include "DistributionFunc.h"

class DistributionLogNormal: public DistributionFunc {
private:
    double mu {0};
    double sigma {0};
public:
    DistributionLogNormal(double mu, double sigma);
    DistributionLogNormal() = default;
};


#endif //COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H
