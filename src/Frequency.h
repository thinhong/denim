//
// Created by thinh on 22/09/2021.
//

#ifndef COMPARTMENT_CPP_FREQUENCY_H
#define COMPARTMENT_CPP_FREQUENCY_H

#include "Distribution.h"

class Frequency: public Distribution {
private:
    double frequency;
public:
    explicit Frequency(double frequency);
    double getTransitionProb(size_t index) override;
    size_t getMaxDay() override;
    std::string getDistName() override;
};


#endif //COMPARTMENT_CPP_FREQUENCY_H
