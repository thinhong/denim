//
// Created by thinh on 08/02/2021.
//

#ifndef MAIN_CPP_DISCRETEWEIBULLDISTRIBUTION_H
#define MAIN_CPP_DISCRETEWEIBULLDISTRIBUTION_H

#include "Distribution.h"

class DiscreteWeibullDistribution: public Distribution {
private:
    std::string distName {"weibull"};
    double scale {0};
    double shape {0};
    size_t maxDay {0};
    std::vector<double> transitionProb;
    void calcTransitionProb();
public:
    DiscreteWeibullDistribution(double scale, double shape);
    std::string getDistName() override;
    double getTransitionProb(size_t index) override;
    size_t getMaxDay() override;
    double getScale();
    double getShape();
};


#endif //MAIN_CPP_DISCRETEWEIBULLDISTRIBUTION_H
