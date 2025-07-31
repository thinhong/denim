//
// Created by thinh on 22/07/2021.
//

#include "ModelJSON.h"
#include <Rcpp.h>


/// @brief constructor for ModelJSON
/// @param initialValues - initial population for each compartment
/// @param parameters - model parameters. hashmap with [parameter name] as key and [parameter value] as value
/// @param transitions - transitions. hashmap with [transtition direction] (e.g. S -> I) as key and [transition] (e.g. d_gammma(3,2)) as value
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
        auto transitionConfig = transition.value();

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
        if (transitionConfig["transition"] == "multinomial") {
            // Make a vector outComps to store separated outCompName i.e. from "I1, I2, I3" to "I1", "I2", "I3"
            std::vector<std::string> outComps;
            std::stringstream outStrings(outCompName);
            std::string segment;
            while (std::getline(outStrings, segment, ',')) {
                outComps.push_back(segment);
            }

            std::weak_ptr<Compartment> inComp = model->getAddressFromName(inCompName);
            std::vector<double> weights = transitionConfig["probabilities"];
            // Manage each out compartment in outComps
            for (size_t i {0}; i < outComps.size(); ++i) {
                std::weak_ptr<Compartment> outComp = model->getAddressFromName(outComps[i]);

                inComp.lock()->addOutCompartment(outComp);
                outComp.lock()->addInCompartment(inComp);
                // Here we add to weight not to probability because we need them to be calculated concurrently
                inComp.lock()->addOutWeight(weights[i]);

                std::shared_ptr<Transition> transitionProb = std::make_shared<TransitionProb>(1);
                inComp.lock()->addOutTransition(transitionProb);

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

            std::shared_ptr<Transition> transition;

            bool dist_init = false;

            // Set transition for the inCompartment
            // If outCompartment is not added, then add the transition, if it has been added then edit the transition
            if (transitionConfig["transition"] == "transitionProb") {
                double prob = transitionConfig["transitionProb"];
                prob *= Transition::timeStep;
                transition = std::make_shared<TransitionProb>(prob);
            }
                // Gamma transition: parameters are "rate" and "shape"
            else if (transitionConfig["transition"] == "gamma") {
                double rate = transitionConfig["rate"];
                double shape = transitionConfig["shape"];
                transition = std::make_shared<TransitionDiscreteGamma>(rate, shape);
                // work around json type cast exception, don't remove
                double tmp_init_config = transitionConfig["dist_init"];
                dist_init = (bool) tmp_init_config;
            }
                // Weibull distribution: parameters are "scale" and "shape"
            else if (transitionConfig["transition"] == "weibull") {
                double scale = transitionConfig["scale"];
                double shape = transitionConfig["shape"];
                transition = std::make_shared<TransitionDiscreteWeibull>(scale, shape);
                // work around json type cast exception, don't remove
                double tmp_init_config = transitionConfig["dist_init"];
                dist_init = (bool) tmp_init_config;
            }
                // Exponential distribution: parameter is "rate"
            else if (transitionConfig["transition"] == "exponential") {
                double rate = transitionConfig["rate"];
                transition = std::make_shared<TransitionDiscreteExponential>(rate);
                // work around json type cast exception, don't remove
                double tmp_init_config = transitionConfig["dist_init"];
                dist_init = (bool) tmp_init_config;
            }
            else if (transitionConfig["transition"] == "lognormal") {
                double mu = transitionConfig["mu"];
                double sigma = transitionConfig["sigma"];
            
                transition = std::make_shared<TransitionLogNormal>(mu, sigma);
                // work around json type cast exception, don't remove
                double tmp_init_config = transitionConfig["dist_init"];
                dist_init = (bool) tmp_init_config;
            }
                // Values transition: parameter is a vector "waitingTime"
            else if (transitionConfig["transition"] == "nonparametric") {
                std::vector<double> waitingTime = transitionConfig["waitingTime"];
                transition = std::make_shared<TransitionNonparametricDist>(waitingTime);
                // work around json type cast exception, don't remove
                double tmp_init_config = transitionConfig["dist_init"];
                dist_init = (bool) tmp_init_config;
            }
            else if (transitionConfig["transition"] == "mathExpression") {
                std::string expression = transitionConfig["expression"];
                transition = std::make_shared<TransitionMathExpression>(expression);
            }
            else if (transitionConfig["transition"] == "constant") {
                double constVal = transitionConfig["constant"];
                transition = std::make_shared<TransitionConstant>(constVal);
            }

            // Set transition for the inCompartment
            // If outCompartment is not added, then add the transition, if it has been added then edit the transition
            if (inComp.lock()->isOutCompAdded(outCompName) == false) {
                // TODO: adjust setting whether to distribute initial value here
                inComp.lock()->addOutTransition(transition, dist_init);
            } else {
                // TODO: also adjust setting whether to distribute initial value here
                inComp.lock()->editOutTransition(outCompName, transition, dist_init);
            }

            // If outCompartment has not been added: set linked compartment in, out, outName, weight
            if (inComp.lock()->isOutCompAdded(outCompName) == false) {
                inComp.lock()->addOutCompartment(outComp);
                outComp.lock()->addInCompartment(inComp);
                inComp.lock()->addOutWeight(weight);
            }
        }
    }

    // Finally, looking back all compartments to see any compartment does not have a transition
    // Impute it with transitionProb = 0
    for (auto& comp: model->getComps()) {
        if (comp->getOutTransitions().empty()) {
            std::shared_ptr<Transition> transitionProb = std::make_shared<TransitionProb>(0);
            comp->addOutTransition(transitionProb);
            comp->addOutWeight(1);
        }

        // normalize outWeight in case weights does not sum up to 1
        // also add outWeight in scenarios where outweight is not explicitly defined 
        comp->normalizeOutWeights();
        // TODO: also distribute starting value based on specified transition
        // TODO: in case of multinomial, create separate chains of subcompartments

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