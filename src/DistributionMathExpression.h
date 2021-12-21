//
// Created by thinh on 15/07/2021.
//

#ifndef MAIN_CPP_MATHEXPRESSION_H
#define MAIN_CPP_MATHEXPRESSION_H

#include "Distribution.h"

class DistributionMathExpression: public Distribution {
private:
    std::string expression;
public:
    explicit DistributionMathExpression(std::string& expression);
    double getTransitionProb(size_t index) override;
    size_t getMaxDay() override;
    std::string getDistName() override;
};


#endif //MAIN_CPP_MATHEXPRESSION_H
