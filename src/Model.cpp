#include "Model.h"
#include "Distribution.h"
#include <algorithm>
#include <stack>
#include <stdexcept>

Model::Model(std::vector<std::string>& paramNames, std::vector<double>& paramValues) {
    this->paramNames = paramNames;
    this->paramValues = paramValues;
}

std::vector<std::shared_ptr<Compartment>> Model::getComps() {
    return comps;
}

void Model::addCompsFromConfig(std::vector<std::shared_ptr<Compartment>> &comps) {
    this->comps = comps;
}

std::weak_ptr<Compartment> Model::getAddressFromName(std::string compName) {
    std::weak_ptr<Compartment> compAddress;
    for (auto& comp: comps) {
        if (comp->getCompName() == compName) {
            compAddress = comp;
        }
    }
    return compAddress;
}

int Model::getIndex(std::shared_ptr<Compartment> comp) {
    auto it = find(comps.begin(), comps.end(), comp);
    int index {-1};
    if (it != comps.end()) {
        index = it - comps.begin();
    }
    return index;
}

void Model::update(size_t iter) {
    // initialize comp total of current iteration with values from previous iteration
    for (auto& comp: this -> comps) {
        comp->updateCompTotal(iter);
    }
    
    for (auto& comp: this -> comps) {
        comp->updateCompartment(iter, paramNames, paramValues, this -> comps);
    }
}

void Model::addCompsOrder(std::string compOrder) {
    // Only add if the comp does not exist in vector compsOrder
    if (std::find(compsOrder.begin(), compsOrder.end(), compOrder) == compsOrder.end()) {
        compsOrder.push_back(compOrder);
    }
}

void Model::sortCompsByInputTransition() {
    std::vector<std::shared_ptr<Compartment>> sortedComps;
    for (auto& compOrder: compsOrder) {
        for (auto& comp: comps) {
            if (comp->getCompName() == compOrder) {
                sortedComps.push_back(comp);
                break;
            }
        }
    }
    comps = sortedComps;
}