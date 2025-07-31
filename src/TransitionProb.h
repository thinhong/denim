//
// Created by thinh on 08/02/2021.
//

#ifndef MAIN_CPP_TRANSITIONPROB_H
#define MAIN_CPP_TRANSITIONPROB_H

#include <memory>
#include "Transition.h"

// Transition probability is used to directly add transitionProb instead of define the distribution of waiting time
class TransitionProb: public Transition {
private:
    double transitionProb;
public:
    explicit TransitionProb(double transitionProb);
    double getTransitionProb(size_t index) override;
    std::vector<double>& getTransitionProbRef() override;
};


#endif //MAIN_CPP_TRANSITIONPROB_H
