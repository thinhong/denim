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
    for (auto& comp: comps) {
        comp->updateCompartment(iter, paramNames, paramValues,
                                allCompNames, allCompValues);
    }
    updateAllCompValues(iter);
}

void Model::initAllComps() {
    allCompNames.clear();
    for (auto& comp: comps) {
        allCompNames.push_back(comp->getCompName());
        allCompValues.push_back(comp->getCompTotal()[0]);
    }
}

void Model::addCompsOrder(std::string compOrder) {
    // Only add if the comp does not exist in vector compsOrder
    if (std::find(compsOrder.begin(), compsOrder.end(), compOrder) == compsOrder.end()) {
        compsOrder.push_back(compOrder);
    }
}

void Model::updateAllCompValues(size_t iter) {
    for (size_t i_comp {0}; i_comp < comps.size(); ++i_comp) {
        allCompNames[i_comp] = comps[i_comp]->getCompName();
        allCompValues[i_comp] = comps[i_comp]->getCompTotal()[iter];
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