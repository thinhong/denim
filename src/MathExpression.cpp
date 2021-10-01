//
// Created by thinh on 15/07/2021.
//

#include "MathExpression.h"

MathExpression::MathExpression(std::string& expression) {
    this->expression = expression;
}

std::string MathExpression::getDistName() {
    return expression;
}

size_t MathExpression::getMaxDay() {
    return 1;
}

double MathExpression::getTransitionProb(size_t index) {
    return 1;
}