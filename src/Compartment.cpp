#include "Compartment.h"

// Constructor
Compartment::Compartment(std::string compName, double initVal) {
    this->compName = compName;
    compTotal.resize(timesFollowUp);
    compTotal[0] = initVal;
}

void Compartment::addOutDistribution(std::shared_ptr<Distribution>& dist, bool distInit) {
    this->outDistributions.push_back(dist);
    this->distSubCompInit.push_back(distInit);
}

void Compartment::editOutDistribution(std::string outName, std::shared_ptr<Distribution> &dist, bool distInit) {
    size_t pos = findOutCompPosition(outName);
    outDistributions[pos] = dist;
    this->distSubCompInit[pos] = distInit;
}

void Compartment::setOutValues() {
    outTotals.resize(outDistributions.size(), 0);
}

// TODO: set length for nested subCompartment
void Compartment::setLengthSubCompartment() {
    size_t maxLength {0};
    // for (auto& outDist: outDistributions) {
    //     if (outDist->getMaxDay() > maxLength) {
    //         maxLength = outDist->getMaxDay();
    //     }
    // }

    // set first dim for subCompartments
    this -> subCompartments.resize(outDistributions.size());

    for (size_t pos = 0; pos < outDistributions.size(); pos++){
        size_t currLength = outDistributions[pos]->getMaxDay();

        // update maxLength, which is used as length for outSubcompartment
        if (currLength > maxLength) {
            maxLength = currLength;
        }

        // update length for subCompartment chain for compartment at i
        this -> subCompartments[pos].resize(currLength, 0);

        // TODO: probably also handle the allocation for initial values here
        // For now just initialize at first subCompartment
        if (!distSubCompInit[pos]){
            // if user specify to not distribute initial value, simply initalize in firs subcompartment
            this -> subCompartments[pos][0] = compTotal[0]*outWeights[pos];
        }else{
            // distribute based on specified outDistribution
            for(size_t i {0}; i<currLength; ++i){
                this-> subCompartments[pos][i] = compTotal[0]*outWeights[pos]*outDistributions[pos]->getProbDist(i);
            }
        }
        
    }
    
    // subCompartments.resize(maxLength);
    // subCompartments[0] = compTotal[0];

    // outSubCompartment is the maximum length of subCompartment
    outSubCompartments.resize(maxLength);
}

/**
 * Initialize compTotal value for current iteration
*/
void Compartment::initCompTotal(size_t iter){
    this -> compTotal[iter] = this -> compTotal[iter - 1];
}

// Helper function to normalize outWeights, to be called at initialization, after defining outCompartments only
void Compartment::normalizeOutWeights(){
    double totalWeight = std::accumulate(this -> outWeights.begin(), this -> outWeights.end(), (double) 0);
    
    for ( size_t pos = 0; pos < outWeights.size(); pos++ ){
        this -> outWeights[pos] = this -> outWeights[pos]/totalWeight;
    }
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
    std::vector<std::string> names;
    for (auto& comp: this->outCompartments) {
        names.push_back(comp.lock()->getCompName());
    }
    return names;
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


void Compartment::addOutWeight(double weight) {
    outWeights.push_back(weight);
}


bool Compartment::isOutCompAdded(std::string nameOutComp) {
    for (auto& comp: this->outCompartments) {
        if (nameOutComp == comp.lock()->getCompName()) {
            return true;
        }
    }
    return false;
}

size_t Compartment::findOutCompPosition(std::string nameOutComp) {
    for (size_t pos = 0; pos < this->outCompartments.size(); pos++){
        if (this->outCompartments[pos].lock()->getCompName() == nameOutComp){
            return pos;
        }
    }
    return 0;
}

/// @brief update compartment attributes for each iteration
/// @param iter current iteration/ time step
/// @param paramNames model parameters
/// @param paramValues model parameters' values
void Compartment::updateCompartment(size_t iter, std::vector<std::string>& paramNames, std::vector<double>& paramValues, std::vector<std::shared_ptr<Compartment>> &comps) {
    // reset out values 
    std::fill(outTotals.begin(), outTotals.end(), 0);

    // Compute total in value of this compartment (later distributed to first subcompartment of each subcompartments chain)
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

    if (!outCompartments.empty()) {
        // loop through each out compartment 
        // update out values and compTotal
        for (size_t outIndex {0}; outIndex < outCompartments.size(); ++outIndex) {
            // reset outSubCompartment for each subCompartment chain
            std::fill(outSubCompartments.begin(), outSubCompartments.end(), 0);

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

            // shift subCompartments for next iteration
            for (size_t i_subComp {subCompartments[outIndex].size() - 1}; i_subComp > 0; --i_subComp) {
                subCompartments[outIndex][i_subComp] = subCompartments[outIndex][i_subComp - 1];

            }
            // update the initial subComp population to be incoming population
            subCompartments[outIndex][0] = outWeights[outIndex]*inValue;
        }
    }

    this -> compTotal[iter] += inValue;
}

/// @brief Update sub compartment with predefined distribution
/// @param iter current iteration
/// @param outIndex index of out compartment
void Compartment::updateSubCompByDist(size_t iter, size_t outIndex) {
    // Going backward from subCompartments[n] -> subCompartments[1]
    // This startIndex is to reduce the number of calculations

    size_t startIndex {0};
    startIndex = std::min(iter, subCompartments[outIndex].size() - 1);
    for (size_t i {0}; i <= startIndex; ++i) {
        // subCompartments[outIndex][startIndex - i] -= outSubCompartments[startIndex - i];
        // update out total 
        outTotals[outIndex] += subCompartments[outIndex][startIndex - i] * outDistributions[outIndex]->getTransitionProb(startIndex - i);
        subCompartments[outIndex][startIndex - i] *= (1 - outDistributions[outIndex] -> getTransitionProb(startIndex - i));
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
    // Add current population in each compartment 
    for (auto &comp: comps){
        parser.DefineConst(comp->getCompName(), comp->getCompTotal()[iter]);
    }

    // The result of this math expression is the outTotals of this outIndex
    // double computeValue = outWeights[outIndex] * parser.Eval(); //deprecated code
    double computeValue = parser.Eval();

    double sumOutTotal = std::accumulate(this -> outTotals.begin(), this -> outTotals.end(), (double) 0);
    
    // To prevent a compartment being negative, only use this value if it + sum of
    // previous out total <= the compTotal of previous iteration
    if (computeValue + sumOutTotal <= compTotal[iter - 1]) {
        outTotals[outIndex] = computeValue;
    } else {
        outTotals[outIndex] = compTotal[iter - 1] - sumOutTotal;
    }

    // Calculate how many people remain in each subCompartment
    double sumSubComp = std::accumulate(this -> subCompartments[outIndex].begin(), this -> subCompartments[outIndex].end(), (double) 0);
    if (sumSubComp > 0) {
        double remainPct = (sumSubComp - outTotals[outIndex]) / sumSubComp;
        for (size_t i_subComp {0}; i_subComp < subCompartments[outIndex].size(); ++i_subComp) {
            subCompartments[outIndex][i_subComp] *= remainPct;
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
    double sumOutTotal = std::accumulate(this -> outTotals.begin(), this -> outTotals.end(), (double) 0);
    // To prevent a compartment being negative, only use this value if it + sum of
    // previous out total <= the compTotal of previous iteration
    if (computeValue + sumOutTotal <= (compTotal[iter - 1] * outWeights[outIndex])) {
        outTotals[outIndex] = computeValue;
    } else {
        outTotals[outIndex] = (compTotal[iter - 1] * outWeights[outIndex]) - sumOutTotal;
    }


    // Calculate how many people remain in each subCompartment
    double sumSubComp = std::accumulate(this->subCompartments[outIndex].begin(), this->subCompartments[outIndex].end(), (double) 0);

    if (sumSubComp > 0) {
        double remainPct = (sumSubComp - outTotals[outIndex]) / sumSubComp;
        for (size_t i_subComp {0}; i_subComp < subCompartments[outIndex].size(); ++i_subComp) {
            subCompartments[outIndex][i_subComp] *= remainPct;
        }
    }

    // Update compTotal after finish this outSubComp
    this -> compTotal[iter] -= outTotals[outIndex];
}

