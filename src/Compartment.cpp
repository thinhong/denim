#include "Compartment.h"

// Constructor
Compartment::Compartment(std::string name, double initVal, std::shared_ptr<Distribution> dist) {
    this->name = name;
    this->dist = dist;
    total.resize(timesFollowUp);
    total[0] = initVal;
    // Each total value is binned into many subCompartmentValues, subCompartmentValues[0] is the initVal
    subCompartmentValues.resize(this->dist->getMaxDay(), 0);
    subCompartmentValues[0] = initVal;
}

// Define list of getters
std::vector<double> Compartment::getTotal() {
    return total;
}
std::vector<double> Compartment::getSubCompartmentValues() {
    return subCompartmentValues;
}
std::string Compartment::getName() {
    return name;
}
std::vector<bool> Compartment::getIsIn() {
    return isIn;
}
std::vector<std::weak_ptr<Compartment>> Compartment::getLinkedCompartment() {
    return linkedCompartment;
}

std::shared_ptr<Distribution> Compartment::getDist() {
    return dist;
}

std::vector<double> Compartment::getLinkedWeight() {
    return linkedWeight;
}

size_t Compartment::getNInNodes() {
    return nInNodes;
}

// Setters
void Compartment::addLinkedCompartment(std::weak_ptr<Compartment> linkedCompartment) {
    this->linkedCompartment.push_back(linkedCompartment);
}

void Compartment::addLinkedWeight(double weight) {
    linkedWeight.push_back(weight);
}

void Compartment::addIsIn(bool isIn) {
    this->isIn.push_back(isIn);
    if (isIn) {
        nInNodes += 1;
    } else {
        nOutNodes += 1;
    }
}

void Compartment::updateValue(long iter, double forceInfection) {
    // Note: the first (S) and last (R, D) compartments must be defined using direct transition prob
    // For all compartments except the first and last compartments
    if (subCompartmentValues.size() > 1) {
        outValue = 0;
        // Going backward from subCompartmentValues[n] -> subCompartmentValues[1]
        size_t startIndex {0};
        if (iter < (subCompartmentValues.size() - 1)) {
            startIndex = iter + 1;
        } else {
            startIndex = subCompartmentValues.size() - 1;
        }
        for (size_t i {startIndex}; i > 0; --i) {
            outValue += subCompartmentValues[i] * dist->getTransitionProb(i);
            subCompartmentValues[i] = subCompartmentValues[i - 1] * (1 - dist->getTransitionProb(i - 1));
        }
        outValue += subCompartmentValues[0] * dist->getTransitionProb(0);
        subCompartmentValues[0] = 0;
        // Loop over all linkedCompartment, find the linkedCompartment with isIn == true
        // Let subCompartmentValues[0] += outValue of that linkedCompartment
        // Multiply with linkedWeight for situations such as A -> Ar and I, I -> H_h, H_c and H_d
        for (size_t j {0}; j < linkedCompartment.size(); ++j) {
            if (isIn[j]) {
                subCompartmentValues[0] += linkedCompartment[j].lock()->outValue * linkedWeight[j];
            }
        }
    }
    // For compartments using direct transition prob
    else if (subCompartmentValues.size() == 1) {
        // First, check if it is the first compartment (S)
        if (nInNodes == 0) {
            outValue = subCompartmentValues[0] * forceInfection;
            subCompartmentValues[0] -= outValue;
        }
        // Then check if it is the last compartment (R or D)
        else {
            outValue = subCompartmentValues[0] * dist->getTransitionProb(0);
            for (size_t j {0}; j < linkedCompartment.size(); ++j) {
                if (isIn[j]) {
                    subCompartmentValues[0] += linkedCompartment[j].lock()->outValue * linkedWeight[j];
                }
            }
            subCompartmentValues[0] -= outValue;
        }
    }

    // Finally sum up subCompartmentValues of this iteration to obtain total value
    for (auto& value: subCompartmentValues) {
        total[iter] += value;
    }
}