//
// Created by thinh on 27/10/2021.
//

#ifndef COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H
#define COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H

#include "Distribution.h"

class DistributionLogNormal: public Distribution {
private:
    double mu {0};
    double sigma {0};
    size_t maxDay {0};
    std::vector<double> transitionProb;
    void calcTransitionProb();
public:
    DistributionLogNormal(double mu, double sigma);
    DistributionLogNormal() = default;
    std::string getDistName() override;
    double getTransitionProb(size_t index) override;
    size_t getMaxDay() override;
};


#endif //COMPARTMENT_CPP_DISTRIBUTIONLOGNORMAL_H
