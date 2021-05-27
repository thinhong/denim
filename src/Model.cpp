#include "Model.h"
#include "Distribution.h"
#include <algorithm>
#include <stack>
#include <stdexcept>

Model::Model(std::vector<std::string> modelGroup, double transmissionRate) {
    for (std::string group: modelGroup) {
        this->modelName.push_back(group);
    }
    this->transmissionRate = transmissionRate * Distribution::timeStep;
}

std::vector<std::string> Model::getModelGroup() {
    return modelName;
}

std::vector<std::shared_ptr<Compartment>> Model::getComps() {
    return comps;
}

double Model::getPopulationSize() {
    return populationSize;
}

void Model::addNewLinkedContactRate(double linkedContactRate) {
    this->linkedContactRates.push_back(linkedContactRate);
}

void Model::updateLinkedContactRate(double linkedContactRateToUpdate, size_t index) {
    linkedContactRates[index] *= linkedContactRateToUpdate;
}

void Model::addLinkedModels(std::vector<std::weak_ptr<Model>> allModels) {
    this->linkedModels = allModels;
}

void Model::addCompsFromConfig(std::vector<std::shared_ptr<Compartment>> &comps) {
    this->comps = comps;
}

std::weak_ptr<Compartment> Model::getAddressFromName(std::string compName) {
    std::weak_ptr<Compartment> compAddress;
    for (auto& comp: comps) {
        if (comp->getName() == compName) {
            compAddress = comp;
        }
    }
    return compAddress;
}

void Model::addCompsAndConnect(std::shared_ptr<Compartment>& A, std::shared_ptr<Compartment>& B, double weight) {
    std::vector<std::string> compName;
    if (!comps.empty()) {
        for (auto& comp: comps) {
            compName.push_back(comp->getName());
        }
    }
    if (!(std::find(compName.begin(), compName.end(), A->getName()) != compName.end())) {
        comps.push_back(A);
    }
    if (!(std::find(compName.begin(), compName.end(), B->getName()) != compName.end())) {
        comps.push_back(B);
    }
    A->addLinkedCompartment(B);
    B->addLinkedCompartment(A);
    A->addIsIn(false);
    B->addIsIn(true);
    A->addLinkedWeight(1);
    B->addLinkedWeight(weight);
}

void Model::addCompsAndConnect2(Compartment &A, Compartment &B, double weight) {
    // std::vector<std::string> compName;
    // if (!comps.empty()) {
    //     for (auto& comp: comps) {
    //         compName.push_back(comp->getName());
    //     }
    // }
    // if (!(std::find(compName.begin(), compName.end(), A.getName()) != compName.end())) {
    //     comps.push_back(A);
    // }
    // if (!(std::find(compName.begin(), compName.end(), B.getName()) != compName.end())) {
    //     comps.push_back(B);
    // }
    A.addLinkedCompartment(std::make_shared<Compartment>(B));
    B.addLinkedCompartment(std::make_shared<Compartment>(A));
    A.addIsIn(false);
    B.addIsIn(true);
    A.addLinkedWeight(1);
    B.addLinkedWeight(weight);
}

void Model::connectComp() {
    for (std::string flow: modelStructure) {
        // Remove whitespace
        flow.erase(remove(flow.begin(), flow.end(), ' '), flow.end());

        int transitionSymbol_pos = flow.find("->");
        // Check whether there is a ":" symbol in this flow
        int probSymbol_pos = flow.find(':');

        // [inComp] [->] [outComp] [:] [prob]
        // inComp start from position 0 and spread from 0 -> transitionSymbol_pos => length = transitionSymbol_pos - 0 = transitionSymbol_pos
        std::string inCompName = flow.substr(0, transitionSymbol_pos);
        // outComp start from transitionSymbol_pos + 2 (transitionSymbol_pos is "->" therefore occupies 2 positions), and
        // spread from transitionSymbol_pos + 2 to probSymbol_pos => length = probSymbol_pos - (transitionSymbol_pos + 2)
        std::string outCompName = flow.substr(transitionSymbol_pos + 2, probSymbol_pos - (transitionSymbol_pos + 2));
        // prob start from probSymbol_pos + 1 and spread to the end of the string
        double weight;
        if (probSymbol_pos != -1) {
            weight = std::stod(flow.substr(probSymbol_pos + 1));
        } else {
            weight = 1;
        }
        std::weak_ptr<Compartment> inComp = this->getAddressFromName(inCompName);
        std::weak_ptr<Compartment> outComp = this->getAddressFromName(outCompName);
        inComp.lock()->addLinkedCompartment(outComp);
        outComp.lock()->addLinkedCompartment(inComp);
        inComp.lock()->addIsIn(false);
        outComp.lock()->addIsIn(true);
        inComp.lock()->addLinkedWeight(weight);
        outComp.lock()->addLinkedWeight(weight);
    }

}

int Model::getIndex(std::shared_ptr<Compartment> comp) {
    auto it = find(comps.begin(), comps.end(), comp);
    int index {-1};
    if (it != comps.end()) {
        index = it - comps.begin();
    }
    return index;
}

int Model::getIndexLinkedModel(std::vector<std::string> modelGroup) {
    int index {0};
    for (size_t i {0}; i < linkedModels.size(); ++i) {
        if (modelGroup == linkedModels[i].lock()->getModelGroup()) {
            index = i;
        }
    }
    return index;
}

std::vector<std::weak_ptr<Model>> Model::getLinkedModels() {
    return linkedModels;
}

bool Model::checkCycleHelper(size_t i, std::vector<bool> &visited, std::vector<bool> &recursiveStack) {
    if (!visited[i]) {
        visited[i] = true;
        recursiveStack[i] = true;
        for (size_t j {0}; j < comps[i]->getLinkedCompartment().size(); ++j) {
            int index = getIndex(comps[i]->getLinkedCompartment()[j].lock());
            if (!comps[i]->getIsIn()[j] && !visited[index] && checkCycleHelper(index, visited, recursiveStack)) {
                return true;
            }
            else if (!comps[i]->getIsIn()[j] && recursiveStack[index]) {
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
    for (size_t j {0}; j < comps[i]->getLinkedCompartment().size(); ++j) {
        if (!comps[i]->getIsIn()[j]) {
            int index = getIndex(comps[i]->getLinkedCompartment()[j].lock());
            if (!visited[index]) {
                sortCompsHelper(index, visited, stack);
            }
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

void Model::calcPopulationSize() {
    for (auto& comp: comps) {
        populationSize += comp->getTotal()[0];
    }
}

double Model::calcForceInfection(size_t iter) {
    // Force of infection: lambda = transmissionRate * contactRates * totalInfectious / N
    double forceInfection {0};
    for (size_t i {0}; i < linkedModels.size(); ++i) {
        double totalInfectious {0};
        for (auto& linkedLocationComp: linkedModels[i].lock()->getComps()) {
            for (std::string& infectiousComp: infectiousComps) {
                if (linkedLocationComp->getName() == infectiousComp) {
                    totalInfectious += linkedLocationComp->getTotal()[iter - 1];
                }
            }
        }
        // Remember to multiply the linkedContactRates
        forceInfection += transmissionRate * linkedContactRates[i] * totalInfectious / linkedModels[i].lock()->getPopulationSize() ;
    }
    return forceInfection;
}

void Model::update(long iter) {
    double forceInfection = calcForceInfection(iter);
    for (auto& comp: comps) {
        comp->updateValue(iter, forceInfection);
    }
}