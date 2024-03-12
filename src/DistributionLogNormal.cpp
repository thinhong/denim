//
// Created by thinh on 27/10/2021.
//

#include "myProb.h"
#include "DistributionLogNormal.h"
#include <iostream>

DistributionLogNormal::DistributionLogNormal(double mu, double sigma) {
    this->mu = mu;
    this->sigma = sigma;
    calcTransitionProb(
        [mu, sigma](double timestep){return log_normal_cdf(timestep, mu, sigma);}
    );
    this->distName = "lognormal";
}
