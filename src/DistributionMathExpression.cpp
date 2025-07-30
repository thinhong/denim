//
// Created by thinh on 15/07/2021.
//

#include "DistributionMathExpression.h"

DistributionMathExpression::DistributionMathExpression(std::string& expression) {
    this->expression = expression;
    this->distName = expression;
    this->maxDay = 1;
}

double DistributionMathExpression::getTransitionProb(size_t index) {
    return 1;
}

std::vector<double>& DistributionMathExpression::getTransitionProbRef() {
    static std::vector<double> returned_val{1};
    return returned_val;
}