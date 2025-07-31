//
// Created by thinh on 15/07/2021.
//

#ifndef MAIN_CPP_MATHEXPRESSION_H
#define MAIN_CPP_MATHEXPRESSION_H

#include "Transition.h"

class TransitionMathExpression: public Transition {
private:
    std::string expression;
public:
    explicit TransitionMathExpression(std::string& expression);
    std::vector<double>& getTransitionProbRef() override;
    double getTransitionProb(size_t index) override;
};


#endif //MAIN_CPP_MATHEXPRESSION_H
