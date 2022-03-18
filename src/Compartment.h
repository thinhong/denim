#ifndef SEAIHCR_COMPARTMENT_H
#define SEAIHCR_COMPARTMENT_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "Distribution.h"
#include "muParser.h"

class Compartment {
private:
    std::string compName;

    // The length of subCompartments is the maximum length of vector transitionProb in outDistributions
    std::vector<double> subCompartments;

    // total: the sum of all subCompartments
    std::vector<double> compTotal;

    // inCompartments: compartments that will move in to this state
    std::vector<std::weak_ptr<Compartment>> inCompartments;

    // outCompartments: compartments that this state will move out, with pre-defined outDistributions and outWeights,
    // after calculation the final output will be sum into outTotals
    std::vector<std::weak_ptr<Compartment>> outCompartments;
    std::vector<std::string> outCompartmentNames;
    std::vector<std::shared_ptr<Distribution>> outDistributions;
    std::vector<double> outWeights;
    std::vector<double> outSubCompartments;
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

    std::vector<double> getSubCompartmentValues() {return subCompartments;};
    std::vector<double> getOutValues() {return outTotals;};
    std::vector<double> getOutSubCompartments() {return outSubCompartments;};

    // Setters
    void addOutDistribution(std::shared_ptr<Distribution>& dist);
    void addOutWeight(double weight);
    void addInCompartment(std::weak_ptr<Compartment>& linkedCompIn);
    void addOutCompartment(std::weak_ptr<Compartment>& linkedCompOut);
    void addOutCompartmentName(std::string& nameOutComp);
    void editOutDistribution(std::string outName, std::shared_ptr<Distribution>& dist);

    // subCompartments and outTotals are set after adding all distributions
    void setLengthSubCompartment();
    void setOutValues();

    size_t findCompPosition(std::vector<std::string>& allCompNames);
    bool isOutCompAdded(std::string nameOutComp);
    size_t findOutCompPosition(std::string nameOutComp);
    void updateAllCompValuesFromComp(size_t iter, std::vector<double>& allCompValues, size_t pos);

    /**
     * Update subCompartments and total at each iteration
     * @param iter
     */
    void updateSubCompByDist(size_t iter, size_t outIndex, std::vector<std::string>& allCompNames, std::vector<double>& allCompValues);

    void updateSubCompByMath(size_t iter, size_t outIndex, std::vector<std::string>& paramNames, std::vector<double>& paramValues,
                             std::vector<std::string>& allCompNames, std::vector<double>& allCompValues);

    void updateSubCompByConst(size_t iter, size_t outIndex, std::vector<std::string>& allCompNames, std::vector<double>& allCompValues);

    void updateCompartment(size_t iter, std::vector<std::string>& paramNames, std::vector<double>& paramValues,
                           std::vector<std::string>& allCompNames, std::vector<double>& allCompValues);
};


#endif //SEAIHCR_COMPARTMENT_H
