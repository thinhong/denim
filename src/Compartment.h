#ifndef SEAIHCR_COMPARTMENT_H
#define SEAIHCR_COMPARTMENT_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "Distribution.h"
#include "muParser.h"
#include <Rcpp.h>

class Compartment {
private:
    std::string compName;
    bool competingRisks; // keep track whether current compartment handles outgoing transitions as competing risk

    // The length of subCompartments is the maximum length of vector transitionProb in outDistributions
    // store population in each sub compartment
    // TODO:
    // - make subcomartments a nested double array, with dim [nOutCompartments, nMaxDwelltime]
    // - instantiate population for subCompartments based on outDistribution
    std::vector<std::deque<double>> subCompartments;

    // total: the sum of all subCompartments (i.e. population of this compartment) per iteration/timestep
    std::vector<double> compTotal;

    // inCompartments: compartments that will move in to this state
    std::vector<std::weak_ptr<Compartment>> inCompartments;

    // outCompartments: compartments that this state will move out to, with pre-defined outDistributions and outWeights,
    // after calculation the final output will be sum into outTotals
    std::vector<std::weak_ptr<Compartment>> outCompartments;
    // out distribution corresponding to each outCompartment
    std::vector<std::shared_ptr<Distribution>> outDistributions; 
    // whether to distribute initial value across subcompartment, same length as outDistributions
    std::vector<bool> distSubCompInit;
    // out weight corresponding to each outCompartment
    std::vector<double> outWeights;
    
    // store population of subCompartment from last iteration for updating
    std::vector<double> tmpSubComp;

    // out population corresponding to each outCompartment in current iteration (sum over all outSubCompartment of that out compartment)
    // values will be reset to 0 then updated while iterating through each timestep
    std::vector<double> outTotals;

public:
    static inline size_t timesFollowUp {200};

    Compartment(std::string name, double initVal);

    Compartment() = delete;
    Compartment(const Compartment& comp) = delete;

    ~Compartment() {
//        std::cout << compName << " compartment destructor called." << std::endl;
    }
    // Getters
    std::vector<double> getCompTotal();
    std::string getCompName();
    std::vector<std::weak_ptr<Compartment>> getInCompartments();
    std::vector<std::weak_ptr<Compartment>> getOutCompartments();
    std::vector<std::string> getOutCompartmentNames();
    std::vector<std::shared_ptr<Distribution>> getOutDistributions();
    std::vector<double> getOutWeights();

    std::vector<std::deque<double>> getSubCompartmentValues() {return subCompartments;};
    std::vector<double> getOutValues() {return outTotals;};

    // Setters
    void addOutDistribution(std::shared_ptr<Distribution>& dist, bool distInit = false);
    void addOutWeight(double weight);
    void addInCompartment(std::weak_ptr<Compartment>& linkedCompIn);
    void addOutCompartment(std::weak_ptr<Compartment>& linkedCompOut);
    void editOutDistribution(std::string outName, std::shared_ptr<Distribution>& dist, bool distInit = false);
    /**
     * Update compTotal value for current iteration
    */
    void initCompTotal(size_t iter);
    // Function to normalize outWeight 
    // used in modelJSON (so for model initialization step only) 
    void normalizeOutWeights();
    // TODO: 
    // Function to distribute initValue of a compartment based on the computed probability distribution 
    // (instead of initValue all in the fist sub-compartment)
    // This function will also be used modelJSON only
    // void distributeInitVal();

    // subCompartments and outTotals are set after adding all distributions
    void setLengthSubCompartment();
    void setOutValues();

    bool isOutCompAdded(std::string nameOutComp);
    size_t findOutCompPosition(std::string nameOutComp);

    /**
     * Update subCompartments and total at each iteration
     * @param iter
     */
    void updateSubCompByDist(size_t iter, size_t outIndex);

    void updateSubCompByMath(size_t iter, size_t outIndex, std::vector<std::string>& paramNames, std::vector<double>& paramValues, std::vector<std::shared_ptr<Compartment>> &comps);

    void updateSubCompByConst(size_t iter, size_t outIndex);

    void updateCompartment(size_t iter, std::vector<std::string>& paramNames, std::vector<double>& paramValues, std::vector<std::shared_ptr<Compartment>> &comps);
};


#endif //SEAIHCR_COMPARTMENT_H
