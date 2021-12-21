//
// Created by thinh on 15/07/2021.
//

#include "DistributionMathExpression.h"

DistributionMathExpression::DistributionMathExpression(std::string& expression) {
    this->expression = expression;
}

std::string DistributionMathExpression::getDistName() {
    return expression;
}

size_t DistributionMathExpression::getMaxDay() {
    return 1;
}

double DistributionMathExpression::getTransitionProb(size_t index) {
    return 1;
}