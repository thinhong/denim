//
// Created by thinh on 22/07/2021.
//

#include "ModelJSON.h"
#include <Rcpp.h>


/// @brief constructor for ModelJSON
/// @param initialValues - initial population for each compartment
/// @param parameters - model parameters. hashmap with [parameter name] as key and [parameter value] as value
/// @param transitions - transitions. hashmap with [transtition direction] (e.g. S -> I) as key and [distribution] (e.g. d_gammma(3,2)) as value
ModelJSON::ModelJSON(nlohmann::ordered_json &initialValues, nlohmann::ordered_json &parameters, nlohmann::ordered_json &transitions) {

    // Set parameters with parameters json
    std::vector<std::string> paramNames;
    std::vector<double> paramValues;
    for (auto& param: parameters.items()) {
        // All keys to paramNames and all values to paramValues
        paramNames.push_back(param.key());
        paramValues.push_back(param.value());
    }
    model = std::make_shared<Model>(paramNames, paramValues);

    // Create all compartments with initialValues json
    std::vector<std::shared_ptr<Compartment>> allCompartments;
    for (auto& initVal: initialValues.items()) {
        std::shared_ptr<Compartment> comp = std::make_shared<Compartment>(initVal.key(), initVal.value());
        allCompartments.push_back(comp);
    }
    model->addCompsFromConfig(allCompartments);

    std::vector<std::string> allInCompNames;
    std::vector<std::string> allOutCompNames;

    // Then add other compartment attributes with the transitions json
    for (auto& transition: transitions.items()) {
        std::string flow = transition.key();
        auto distributionConfig = transition.value();

        // Remove whitespace
        flow.erase(remove(flow.begin(), flow.end(), ' '), flow.end());

        // We need to extract inComp, outComp and weight from the string
        std::string inCompName;
        std::string outCompName;
        double weight {1};

        // Find [->] symbol
        unsigned long transitionSymbol_pos = flow.find("->");

        // OutCompName is always everything after the [->] symbol
        outCompName = flow.substr(transitionSymbol_pos + 2);

        //======================== Manage the inCompName part ========================//
        // Check if there is a [*] symbol
        if (flow.find('*') == std::string::npos) {
            inCompName = flow.substr(0, transitionSymbol_pos);
        } else {
            // Find the proportion [*] symbol
            unsigned long propSymbol_pos = flow.find('*');

            // Get 3 separated strings out of that flow s1 [*] s2 [->] outCompName
            std::string s1;
            std::string s2;

            s1 = flow.substr(0, propSymbol_pos);
            s2 = flow.substr(propSymbol_pos + 1, transitionSymbol_pos - (propSymbol_pos + 1));

            // Check s1 or s2 is number
            // If both are number
            if (std::strspn(s1.c_str(), "-.0123456789") == s1.size() &&
                std::strspn(s2.c_str(), "-.0123456789") == s2.size()) {
                Rcpp::Rcout << "Compartment name must be string characters, not number" << std::endl;
            }
            // If s1 is number and s2 not number
            else if (std::strspn(s1.c_str(), "-.0123456789") == s1.size()) {
                weight = std::stod(s1);
                inCompName = s2;
            }
            // If s2 is number
            else if (std::strspn(s2.c_str(), "-.0123456789") == s2.size()) {
                weight = std::stod(s2);
                inCompName = s1;
            }
            // Else if both are not number
            else {
                Rcpp::Rcout << "Proportion must be a number" << "\n";
            }
        }

        // Add this inComp to the allInCompNames vector
        allInCompNames.push_back(inCompName);

        //======================== Manage the outCompName part ========================//
        // Special scenario "multinomial": outCompName is a string of multiple names separated by [,], e.g. "I1, I2, I3"
        if (distributionConfig["distribution"] == "multinomial") {
            // Make a vector outComps to store separated outCompName i.e. from "I1, I2, I3" to "I1", "I2", "I3"
            std::vector<std::string> outComps;
            std::stringstream outStrings(outCompName);
            std::string segment;
            while (std::getline(outStrings, segment, ',')) {
                outComps.push_back(segment);
            }

            std::weak_ptr<Compartment> inComp = model->getAddressFromName(inCompName);
            std::vector<double> weights = distributionConfig["probabilities"];
            // Manage each out compartment in outComps
            for (size_t i {0}; i < outComps.size(); ++i) {
                std::weak_ptr<Compartment> outComp = model->getAddressFromName(outComps[i]);

                inComp.lock()->addOutCompartment(outComp);
                inComp.lock()->addOutCompartmentName(outComps[i]); 
                outComp.lock()->addInCompartment(inComp);
                // Here we add to weight not to probability because we need them to be calculated concurrently
                inComp.lock()->addOutWeight(weights[i]);

                std::shared_ptr<Distribution> transitionProb = std::make_shared<DistributionTransitionProb>(1);
                inComp.lock()->addOutDistribution(transitionProb);

                // Add the outComp to compsOrder
                allOutCompNames.push_back(outComps[i]);
            }
        }
        // If not, continue to use outCompName as normal
        else {
            // Add the outComp to compsOrder
            allOutCompNames.push_back(outCompName);

            // get address of in and out compartment
            std::weak_ptr<Compartment> inComp = model->getAddressFromName(inCompName);
            std::weak_ptr<Compartment> outComp = model->getAddressFromName(outCompName);

            std::shared_ptr<Distribution> distribution;

            // Set distribution for the inCompartment
            // If outCompartment is not added, then add the distribution, if it has been added then edit the distribution
            if (distributionConfig["distribution"] == "transitionProb") {
                double prob = distributionConfig["transitionProb"];
                prob *= Distribution::timeStep;
                distribution = std::make_shared<DistributionTransitionProb>(prob);
            }
                // Gamma distribution: parameters are "scale" and "shape"
            else if (distributionConfig["distribution"] == "gamma") {
                double scale = distributionConfig["scale"];
                double shape = distributionConfig["shape"];
                distribution = std::make_shared<DistributionDiscreteGamma>(scale, shape);
            }
                // Weibull distribution: parameters are "scale" and "shape"
            else if (distributionConfig["distribution"] == "weibull") {
                double scale = distributionConfig["scale"];
                double shape = distributionConfig["shape"];
                distribution = std::make_shared<DistributionDiscreteWeibull>(scale, shape);
            }
                // Exponential distribution: parameter is "rate"
            else if (distributionConfig["distribution"] == "exponential") {
                double rate = distributionConfig["rate"];
                distribution = std::make_shared<DistributionDiscreteExponential>(rate);
            }
            else if (distributionConfig["distribution"] == "lognormal") {
                double mu = distributionConfig["mu"];
                double sigma = distributionConfig["sigma"];
                
                distribution = std::make_shared<DistributionLogNormal>(mu, sigma);
            }
                // Values distribution: parameter is a vector "waitingTime"
            else if (distributionConfig["distribution"] == "nonparametric") {
                std::vector<double> waitingTime = distributionConfig["waitingTime"];
                distribution = std::make_shared<DistributionNonparametric>(waitingTime);
            }
            else if (distributionConfig["distribution"] == "mathExpression") {
                std::string expression = distributionConfig["expression"];
                distribution = std::make_shared<DistributionMathExpression>(expression);
            }
            else if (distributionConfig["distribution"] == "constant") {
                double constVal = distributionConfig["constant"];
                distribution = std::make_shared<DistributionConstant>(constVal);
            }

            // Set distribution for the inCompartment
            // If outCompartment is not added, then add the distribution, if it has been added then edit the distribution
            if (inComp.lock()->isOutCompAdded(outCompName) == false) {
                inComp.lock()->addOutDistribution(distribution);
            } else {
                inComp.lock()->editOutDistribution(outCompName, distribution);
            }

            // If outCompartment has not been added: set linked compartment in, out, outName, weight
            if (inComp.lock()->isOutCompAdded(outCompName) == false) {
                inComp.lock()->addOutCompartment(outComp);
                inComp.lock()->addOutCompartmentName(outCompName);
                outComp.lock()->addInCompartment(inComp);
                inComp.lock()->addOutWeight(weight);
            }
        }
    }

    // Finally, looking back all compartments to see any compartment does not have a distribution
    // Impute it with transitionProb = 0
    for (auto& comp: model->getComps()) {
        if (comp->getOutDistributions().empty()) {
            std::shared_ptr<Distribution> transitionProb = std::make_shared<DistributionTransitionProb>(0);
            comp->addOutDistribution(transitionProb);
            comp->addOutWeight(1);
        }
        comp->setLengthSubCompartment();
        comp->setOutValues();
    }

    // Add compOrder: allInCompNames first, and then allOutCompNames
    for (auto& inName: allInCompNames) {
        model->addCompsOrder(inName);
    }
    for (auto& outName: allOutCompNames) {
        model->addCompsOrder(outName);
    }
}

std::shared_ptr<Model> ModelJSON::getModel() {
    return model;
}