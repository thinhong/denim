#include "Compartment.h"

// Constructor
Compartment::Compartment(std::string compName, double initVal) {
    this->compName = compName;
    compTotal.resize(timesFollowUp);
    compTotal[0] = initVal;
}

void Compartment::addOutDistribution(std::shared_ptr<Distribution>& dist) {
    this->outDistributions.push_back(dist);
}

void Compartment::editOutDistribution(std::string outName, std::shared_ptr<Distribution> &dist) {
    size_t pos = findOutCompPosition(outName);
    outDistributions[pos] = dist;
}

void Compartment::setOutValues() {
    outTotals.resize(outDistributions.size(), 0);
}

void Compartment::setLengthSubCompartment() {
    size_t maxLength {0};
    for (auto& outDist: outDistributions) {
        if (outDist->getMaxDay() > maxLength) {
            maxLength = outDist->getMaxDay();
        }
    }
    subCompartments.resize(maxLength);
    subCompartments[0] = compTotal[0];
    // outSubCompartment is a clone of subCompartments
    outSubCompartments.resize(maxLength);
}

void Compartment::updateCompTotal(size_t iter){
    this -> compTotal[iter] = this -> compTotal[iter - 1];
}

// Define list of getters
std::string Compartment::getCompName() {
    return compName;
}

std::vector<std::weak_ptr<Compartment>> Compartment::getInCompartments() {
    return inCompartments;
}

std::vector<std::weak_ptr<Compartment>> Compartment::getOutCompartments() {
    return outCompartments;
}

std::vector<std::string> Compartment::getOutCompartmentNames() {
    return outCompartmentNames;
}

std::vector<std::shared_ptr<Distribution>> Compartment::getOutDistributions() {
    return outDistributions;
}

std::vector<double> Compartment::getOutWeights() {
    return outWeights;
}

std::vector<double> Compartment::getCompTotal() {
    return compTotal;
}

// Setters
void Compartment::addInCompartment(std::weak_ptr<Compartment>& linkedCompIn) {
    this->inCompartments.push_back(linkedCompIn);
}

void Compartment::addOutCompartment(std::weak_ptr<Compartment>& linkedCompOut) {
    this->outCompartments.push_back(linkedCompOut);
}

void Compartment::addOutCompartmentName(std::string &nameOutComp) {
    this->outCompartmentNames.push_back(nameOutComp);
}

void Compartment::addOutWeight(double weight) {
    outWeights.push_back(weight);
}


bool Compartment::isOutCompAdded(std::string nameOutComp) {
    bool exist {false};
    for (auto& outName: outCompartmentNames) {
        if (nameOutComp == outName) {
            exist = true;
            break;
        }
    }
    return exist;
}

size_t Compartment::findOutCompPosition(std::string nameOutComp) {
    size_t pos = std::find(outCompartmentNames.begin(), outCompartmentNames.end(), nameOutComp) - outCompartmentNames.begin();
    return pos;
}

void Compartment::updateAllCompValuesFromComp(size_t iter, std::vector<double> &allCompValues, size_t pos) {
    // update value of current comp in model's all comp values
    allCompValues[pos] = compTotal[iter];
}

/// @brief update compartment attributes for each iteration
/// @param iter current iteration/ time step
/// @param paramNames model parameters
/// @param paramValues model parameters' values
void Compartment::updateCompartment(size_t iter, std::vector<std::string>& paramNames, std::vector<double>& paramValues, std::vector<std::shared_ptr<Compartment>> &comps) {

    // compTotal[iter] = compTotal[iter - 1];
    std::fill(outSubCompartments.begin(), outSubCompartments.end(), 0);
    std::fill(outTotals.begin(), outTotals.end(), 0);

    if (!outCompartments.empty()) {
        // loop through each out compartment 
        // update out values and compTotal

        for (size_t outIndex {0}; outIndex < outCompartments.size(); ++outIndex) {
            if (outDistributions[outIndex]->getDistName() == "gamma" ||
                outDistributions[outIndex]->getDistName() == "weibull" ||
                outDistributions[outIndex]->getDistName() == "exponential" ||
                outDistributions[outIndex]->getDistName() == "lognormal" ||
                outDistributions[outIndex]->getDistName() == "transitionProb" ||
                outDistributions[outIndex]->getDistName() == "nonparametric") {
                updateSubCompByDist(iter, outIndex);
            } else if (outDistributions[outIndex]->getDistName() == "constant") {
                updateSubCompByConst(iter, outIndex);
            } else {
                updateSubCompByMath(iter, outIndex, paramNames, paramValues, comps);
            }
        }
    }

    // update population of each sub compartment
    if (subCompartments.size() == 1) {
        subCompartments[0] -= outSubCompartments[0];
    } else {
        size_t n_subComp = subCompartments.size() - 1;
        for (size_t i_subComp {n_subComp}; i_subComp > 0; --i_subComp) {
            subCompartments[i_subComp] = subCompartments[i_subComp - 1] - outSubCompartments[i_subComp - 1];
        }
        subCompartments[0] = 0;
    }

    // Add the inValue to subCompartments[0]
    double inValue {0};
    // Loop over all inCompartments
    for (size_t inIndex {0}; inIndex < inCompartments.size(); ++inIndex) {
        // Find the outCompartments of these inCompartments
        for (size_t outOfIn {0}; outOfIn < inCompartments[inIndex].lock()->outCompartments.size(); ++outOfIn) {
            // Based on name, find the outValue that correspond to this compartment and add to inValue
            if (compName == inCompartments[inIndex].lock()->outCompartments[outOfIn].lock()->getCompName()) {
                inValue += inCompartments[inIndex].lock()->outTotals[outOfIn];
            }
        }
    }
    subCompartments[0] += inValue;
    this -> compTotal[iter] += inValue;
}

/// @brief Update sub compartment with predefined distribution
/// @param iter current iteration
/// @param outIndex index of out compartment
void Compartment::updateSubCompByDist(size_t iter, size_t outIndex) {
    outTotals[outIndex] = 0;
    // Going backward from subCompartments[n] -> subCompartments[1]
    // This startIndex is to reduce the number of calculations
    size_t startIndex {0};
    startIndex = std::min(iter, subCompartments.size() - 1);
        
    // Put if outside to check condition only once
    if (outWeights[outIndex] == 1) {
        for (size_t i {0}; i <= startIndex; ++i) {
            subCompartments[startIndex - i] -= outSubCompartments[startIndex - i];
            // udpate total 
            // std::cout << "Calculating distribution " << outDistributions[outIndex] -> getDistName() <<std::endl;
            // std::cout << "Transition prob at index " << (startIndex - i) << ": " << outDistributions[outIndex] -> getTransitionProb(startIndex - i) <<std::endl;
            outTotals[outIndex] += subCompartments[startIndex - i] * outDistributions[outIndex]->getTransitionProb(startIndex - i);
            subCompartments[startIndex - i] *= (1 - outDistributions[outIndex] -> getTransitionProb(startIndex - i));
        }
        // After finishing, clean the outSubCompartments vector
        std::fill(outSubCompartments.begin(), outSubCompartments.end(), 0);
    } else if (outWeights[outIndex] < 1) {
        for (size_t i {0}; i <= startIndex; ++i) {
            outTotals[outIndex] += outWeights[outIndex] * subCompartments[startIndex - i] * outDistributions[outIndex]->getTransitionProb(startIndex - i);
            outSubCompartments[startIndex - i] += outWeights[outIndex] * subCompartments[startIndex - i] * outDistributions[outIndex]->getTransitionProb(startIndex - i);
        }
    }

    // Update compTotal after finish this outSubComp
    this -> compTotal[iter] -= outTotals[outIndex];
}


/// @brief Update sub compartment with math expression as distribution
/// @param iter current iteration
/// @param outIndex index of out compartment
/// @param paramNames model parameteres
/// @param paramValues value of model parameters
/// @param comps all model's compartments, required to get the current population of compartents
void Compartment::updateSubCompByMath(size_t iter, size_t outIndex, std::vector<std::string>& paramNames, std::vector<double>& paramValues,
                std::vector<std::shared_ptr<Compartment>> &comps) {
    mu::Parser parser;
    parser.SetExpr(outDistributions[outIndex]->getDistName());
    // Add parameter values
    for (size_t i {0}; i < paramNames.size(); ++i) {
        parser.DefineConst(paramNames[i], paramValues[i]);
    }

    for (auto &comp: comps){
        // if(iter < 3){
        //     std::cout << comp->getCompName() << " val from compartment compTotal " << comp->getCompTotal()[iter] << std::endl;
        // }
        parser.DefineConst(comp->getCompName(), comp->getCompTotal()[iter]);
    }
    // The result of this math expression is the outTotals of this outIndex
    double computeValue = outWeights[outIndex] * parser.Eval();

    double sumOutTotal {0};
    for (auto& outTotal: outTotals) {
        sumOutTotal += outTotal;
    }

    // To prevent a compartment being negative, only use this value if it + sum of
    // previous out total <= the compTotal of previous iteration
    if (computeValue + sumOutTotal <= compTotal[iter - 1]) {
        outTotals[outIndex] = computeValue;
    } else {
        outTotals[outIndex] = compTotal[iter - 1] - sumOutTotal;
    }

    // If outWeight = 1 then calculate directly in the subCompartment
    size_t startIndex {0};
    startIndex = std::min(iter, subCompartments.size() - 1);

    if (outWeights[outIndex] == 1) {
        for (size_t i {0}; i <= startIndex; ++i) {
            subCompartments[startIndex - i] -= outSubCompartments[startIndex - i];
        }
        // After finishing, clean the outSubCompartments vector
        std::fill(outSubCompartments.begin(), outSubCompartments.end(), 0);

        // Calculate how many people remain in each subCompartment
        double sumSubComp {0};
        for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
            sumSubComp += subCompartments[i_subComp];
        }
        if (sumSubComp > 0) {
            double remainPct = (sumSubComp - outTotals[outIndex]) / sumSubComp;
            for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
                subCompartments[i_subComp] *= remainPct;
            }
        }
    } else if (outWeights[outIndex] < 1) {
        // If weight < 1 then perform it on the outSubCompartments
        double sumSubComp {0};
        for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
            sumSubComp += subCompartments[i_subComp];
        }
        // Because sumSubComp is the denominator, sumSubComp = 0 this formula returns error (not a number)
        if (sumSubComp > 0) {
            double outPct = outTotals[outIndex] / sumSubComp;
            for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
                outSubCompartments[i_subComp] += outPct * subCompartments[i_subComp];
            }
        }
    }

    // Update compTotal after finish this outSubComp
    this -> compTotal[iter] -= outTotals[outIndex];
}

/// @brief Update sub compartment with constant transition rate
/// @param iter current iteration
/// @param outIndex index of out compartment
void Compartment::updateSubCompByConst(size_t iter, size_t outIndex) {

    double computeValue = outDistributions[outIndex]->getTransitionProb(iter);

    double sumOutTotal {0};
    for (auto& outTotal: outTotals) {
        sumOutTotal += outTotal;
    }

    // To prevent a compartment being negative, only use this value if it + sum of
    // previous out total <= the compTotal of previous iteration
    if (computeValue + sumOutTotal <= (compTotal[iter - 1] * outWeights[outIndex])) {
        outTotals[outIndex] = computeValue;
    } else {
        outTotals[outIndex] = (compTotal[iter - 1] * outWeights[outIndex]) - sumOutTotal;
    }

    // If outWeight = 1 then calculate directly in the subCompartment
    size_t startIndex {0};
    startIndex = std::min(iter, subCompartments.size() - 1);

    if (outWeights[outIndex] == 1) {
        for (size_t i {0}; i <= startIndex; ++i) {
            subCompartments[startIndex - i] -= outSubCompartments[startIndex - i];
        }
        // After finishing, clean the outSubCompartments vector
        std::fill(outSubCompartments.begin(), outSubCompartments.end(), 0);

        // Calculate how many people remain in each subCompartment
        double sumSubComp {0};
        for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
            sumSubComp += subCompartments[i_subComp];
        }
        if (sumSubComp > 0) {
            double remainPct = (sumSubComp - outTotals[outIndex]) / sumSubComp;
            for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
                subCompartments[i_subComp] *= remainPct;
            }
        }
    } else if (outWeights[outIndex] < 1) {
        // If weight < 1 then perform it on the outSubCompartments
        double sumSubComp {0};
        for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
            sumSubComp += subCompartments[i_subComp];
        }
        if (sumSubComp > 0) {
            double outPct = outTotals[outIndex] / sumSubComp;
            for (size_t i_subComp {0}; i_subComp < subCompartments.size(); ++i_subComp) {
                outSubCompartments[i_subComp] += outPct * subCompartments[i_subComp];
            }
        }
    }

    // Update compTotal after finish this outSubComp
    this -> compTotal[iter] -= outTotals[outIndex];
}

