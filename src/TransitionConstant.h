//
// Created by thinh on 22/09/2021.
//

#ifndef COMPARTMENT_CPP_DISTRIBUTIONCONSTANT_H
#define COMPARTMENT_CPP_DISTRIBUTIONCONSTANT_H

#include "Transition.h"

class TransitionConstant: public Transition {
private:
    double constant;
public:
    explicit TransitionConstant(double constValue);
    double getTransitionProb(size_t index) override;
    std::vector<double>& getTransitionProbRef() override;
};


#endif //COMPARTMENT_CPP_DISTRIBUTIONCONSTANT_H
