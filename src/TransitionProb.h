//
// Created by thinh on 08/02/2021.
//

#ifndef MAIN_CPP_TRANSITIONPROB_H
#define MAIN_CPP_TRANSITIONPROB_H

#include <memory>
#include "Distribution.h"

// Transition probability is used to directly add transitionProb instead of define the distribution of waiting time
class TransitionProb: public Distribution {
private:
    std::string distName {"transitionProb"};
    double transitionProb;
public:
    explicit TransitionProb(double transitionProb);
    double getTransitionProb(size_t index) override;
    size_t getMaxDay() override;
    std::string getDistName() override;
};


#endif //MAIN_CPP_TRANSITIONPROB_H
