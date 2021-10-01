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

bool Model::checkCycleHelper(size_t i, std::vector<bool> &visited, std::vector<bool> &recursiveStack) {
    if (!visited[i]) {
        visited[i] = true;
        recursiveStack[i] = true;
        for (size_t j {0}; j < comps[i]->getOutCompartments().size(); ++j) {
            int index = getIndex(comps[i]->getOutCompartments()[j].lock());
            if (!visited[index] && checkCycleHelper(index, visited, recursiveStack)) {
                return true;
            }
            else if (recursiveStack[index]) {
                return true;
            }
        }
    }
    recursiveStack[i] = false;
    return false;
}

void Model::checkCycle() {
    std::vector<bool> visited;
    visited.resize(comps.size(), false);
    std::vector<bool> recursiveStack;
    recursiveStack.resize(comps.size(), false);
    for (size_t i {0}; i < comps.size(); ++i) {
        try {
            if (checkCycleHelper(i, visited, recursiveStack)) {
                throw std::logic_error("A cycle exists in your model");
            }
        }
        catch (std::logic_error& e) {
            std::terminate();
        }
    }
}

void Model::sortCompsHelper(size_t i, std::vector<bool> &visited, std::stack<std::shared_ptr<Compartment>> &stack) {
    visited[i] = true;
    for (size_t j {0}; j < comps[i]->getOutCompartments().size(); ++j) {
        int index = getIndex(comps[i]->getOutCompartments()[j].lock());
        if (!visited[index]) {
            sortCompsHelper(index, visited, stack);
        }
    }
    stack.push(comps[i]);
}

void Model::sortComps() {
    checkCycle();
    std::stack<std::shared_ptr<Compartment>> stack;
    std::vector<std::shared_ptr<Compartment>> sortedComps;
    std::vector<bool> visited;
    visited.resize(comps.size(), false);
    for (size_t i{0}; i < comps.size(); ++i) {
        if (!visited[i]) {
            sortCompsHelper(i, visited, stack);
        }
    }
    while (!stack.empty()) {
        sortedComps.push_back(stack.top());
        stack.pop();
    }
    comps = sortedComps;
}

void Model::update(long iter) {
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

void Model::updateAllCompValues(size_t iter) {
    for (size_t i_comp {0}; i_comp < comps.size(); ++i_comp) {
        allCompNames[i_comp] = comps[i_comp]->getCompName();
        allCompValues[i_comp] = comps[i_comp]->getCompTotal()[iter];
    }
}