//
// Created by thinh on 22/07/2021.
//

#include "ModelJSON.h"

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

    // Then add other compartment attributes with the transitions json
    for (auto& transition: transitions.items()) {
        std::string flow = transition.key();
        auto distributionConfig = transition.value();
        // Remove whitespace
        flow.erase(remove(flow.begin(), flow.end(), ' '), flow.end());

        unsigned long transitionSymbol_pos = flow.find("->");
        // Check whether there is a ":" symbol in this flow
        unsigned long probSymbol_pos = flow.find('*');
        std::string inCompName;
        std::string outCompName;
        double weight;

        // [inComp] [->] [outComp] [:] [prob]
        // inComp start from position 0 and spread from 0 -> transitionSymbol_pos => length = transitionSymbol_pos - 0 = transitionSymbol_pos
        inCompName = flow.substr(0, transitionSymbol_pos);
        if (probSymbol_pos != -1) {
            outCompName = flow.substr(probSymbol_pos + 1);
            weight = std::stod(flow.substr(transitionSymbol_pos + 2, probSymbol_pos - (transitionSymbol_pos + 2)));
        } else {
            // If no weight is defined (S -> I), outCompName start from transitionSymbol_pos + 2 and spans to the end
            outCompName = flow.substr(transitionSymbol_pos + 2);
            weight = 1;
        }
        std::weak_ptr<Compartment> inComp = model->getAddressFromName(inCompName);
        std::weak_ptr<Compartment> outComp = model->getAddressFromName(outCompName);

        // Set linked compartment in and out and their weight
        inComp.lock()->addOutCompartment(outComp);
        outComp.lock()->addInCompartment(inComp);
        inComp.lock()->addOutWeight(weight);

        // Set distribution for the inCompartment
        if (distributionConfig["distribution"] == "transitionProb") {
            double prob = distributionConfig["transitionProb"];
            prob *= Distribution::timeStep;
            std::shared_ptr<Distribution> transitionProb = std::make_shared<TransitionProb>(prob);
            inComp.lock()->addOutDistribution(transitionProb);
        }
            // Gamma distribution: parameters are "scale" and "shape"
        else if (distributionConfig["distribution"] == "gamma") {
            double scale = distributionConfig["scale"];
            scale /= Distribution::timeStep;
            double shape = distributionConfig["shape"];
            std::shared_ptr<Distribution> gamma = std::make_shared<DiscreteGammaDistribution>(scale, shape);
            inComp.lock()->addOutDistribution(gamma);
        }
            // Weibull distribution: parameters are "scale" and "shape"
        else if (distributionConfig["distribution"] == "weibull") {
            double scale = distributionConfig["scale"];
            scale /= Distribution::timeStep;
            double shape = distributionConfig["shape"];
            std::shared_ptr<Distribution> weibull = std::make_shared<DiscreteWeibullDistribution>(scale, shape);
            inComp.lock()->addOutDistribution(weibull);
        }
            // Exponential distribution: parameter is "rate"
        else if (distributionConfig["distribution"] == "exponential") {
            double rate = distributionConfig["rate"];
            rate *= Distribution::timeStep;
            std::shared_ptr<Distribution> exponential = std::make_shared<DiscreteExponentialDistribution>(rate);
            inComp.lock()->addOutDistribution(exponential);
        }
            // Values distribution: parameter is a vector "waitingTime"
        else if (distributionConfig["distribution"] == "values") {
            std::vector<double> waitingTime = distributionConfig["waitingTime"];
            std::shared_ptr<Distribution> values = std::make_shared<ValuesDistribution>(waitingTime);
            inComp.lock()->addOutDistribution(values);
        }
        else if (distributionConfig["distribution"] == "mathExpression") {
            std::string expression = distributionConfig["expression"];
            std::shared_ptr<Distribution> mathExpression = std::make_shared<MathExpression>(expression);
            inComp.lock()->addOutDistribution(mathExpression);
        }
        else if (distributionConfig["distribution"] == "frequency") {
            double freq = distributionConfig["frequency"];
            std::shared_ptr<Distribution> frequency = std::make_shared<Frequency>(freq);
            inComp.lock()->addOutDistribution(frequency);
        }
    }
    for (auto& comp: model->getComps()) {
        if (comp->getOutDistributions().empty()) {
            double prob = 0.0;
            std::shared_ptr<Distribution> transitionProb = std::make_shared<TransitionProb>(prob);
            comp->addOutDistribution(transitionProb);
            comp->addOutWeight(1);
        }
        comp->setLengthSubCompartment();
        comp->setOutValues();
    }
}

std::shared_ptr<Model> ModelJSON::getModel() {
    return model;
}