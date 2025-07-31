//
// Created by thinh on 04/03/2021.
//

#ifndef MAIN_CPP_CUSTOMDISTRIBUTION_H
#define MAIN_CPP_CUSTOMDISTRIBUTION_H

#include <string>
#include <vector>
#include "Transition.h"

class TransitionNonparametricDist: public Transition {
private:
    std::vector<double> waitingTime;
    std::vector<double> transitionProb;
    void calcTransitionProb();
public:
    explicit TransitionNonparametricDist(std::vector<double> waitingTime);
    double getTransitionProb(size_t index) override;
    std::vector<double>& getTransitionProbRef() override;
    double getProbDist(size_t index) override;
    std::vector<double> getWaitingTime();
};


#endif //MAIN_CPP_CUSTOMDISTRIBUTION_H
