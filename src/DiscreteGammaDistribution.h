//
// Created by thinh on 05/02/2021.
//

#ifndef MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H
#define MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H

#include "Distribution.h"

class DiscreteGammaDistribution: public Distribution {
private:
    std::string distName {"gamma"};
    double scale {0};
    double shape {0};
    size_t maxDay {0};
    std::vector<double> transitionProb;
    void calcTransitionProb();
public:
    DiscreteGammaDistribution(double scale, double shape);
    explicit DiscreteGammaDistribution(std::vector<double>& cumulativeProb);
    DiscreteGammaDistribution() = default;
    std::string getDistName() override;
    double getTransitionProb(size_t index) override;
    size_t getMaxDay() override;
    double getScale();
    double getShape();
};


#endif //MAIN_CPP_DISCRETEGAMMADISTRIBUTION_H
