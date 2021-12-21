#include "helpers.h"

std::vector<std::string> extractCompNames(std::string element) {
    std::vector<std::string> compNames;
    // Remove all whitespace
    element.erase(remove(element.begin(), element.end(), ' '), element.end());
    // Find the location of "->"
    int transitionSymbol_pos = element.find("->");
    // Check whether there is a ":" symbol in this flow
    int probSymbol_pos = element.find(':');
    // inComp start from position 0 and spread from 0 -> transitionSymbol_pos => length = transitionSymbol_pos - 0 = transitionSymbol_pos
    std::string inCompName = element.substr(0, transitionSymbol_pos);
    // outComp start from transitionSymbol_pos + 2 (transitionSymbol_pos is "->" therefore occupies 2 positions), and
    // spread from transitionSymbol_pos + 2 to probSymbol_pos => length = probSymbol_pos - (transitionSymbol_pos + 2)
    std::string outCompName = element.substr(transitionSymbol_pos + 2, probSymbol_pos - (transitionSymbol_pos + 2));
    compNames.insert(compNames.end(), {inCompName, outCompName});
    return compNames;
}

void viewModelStructure(std::shared_ptr<Model> model) {
    for (auto& comp: model->getComps()) {
        std::cout << "Compartment " << comp->getCompName() << "\n";

        std::cout << "In compartments: ";
        for (auto& inComp: comp->getInCompartments()) {
            std::cout << inComp.lock()->getCompName() << " ";
        }
        std::cout << "\n";

        std::cout << "Out compartments (derived from pointer): ";
        for (auto& outComp: comp->getOutCompartments()) {
            std::cout << outComp.lock()->getCompName() << " ";
        }
        std::cout << "\n";

        std::cout << "Out compartments (direct from outCompartmentNames): ";
        for (auto& outComp: comp->getOutCompartmentNames()) {
            std::cout << outComp << " ";
        }
        std::cout << "\n";

        std::cout << "Out distribution: ";
        for (auto& outDist: comp->getOutDistributions()) {
            std::cout << outDist << " (" << outDist->getDistName() << "), ";
        }
        std::cout << "\n";

        std::cout << "Out weights: ";
        for (auto& outWeight: comp->getOutWeights()) {
            std::cout << outWeight << " ";
        }
        std::cout << "\n";

        std::cout << "============" << "\n";
    }
}

void viewModelUpdate(std::shared_ptr<Model> model, long iter) {
    std::cout << "====================================" << "\n";
    std::cout << "Iteration " << iter << "\n";
    for (auto& comp: model->getComps()) {
        std::cout << "Compartment " << comp->getCompName() << "\n";
        for (size_t k {0}; k < comp->getSubCompartmentValues().size(); ++k) {
            std::cout << comp->getSubCompartmentValues()[k] << " ";
        }
        std::cout << "\n";
        for (size_t j {0}; j < comp->getOutCompartments().size(); ++j) {
            std::cout << "Out value for " << comp->getOutCompartments()[j].lock()->getCompName() << ": " <<
                      comp->getOutValues()[j] << "\n";
        }
        std::cout << "Total " << comp->getCompName() << ": ";
        for (auto& total: comp->getCompTotal()) {
            std::cout << total << " ";
        }
        std::cout << "\n";
    }
}

std::vector<std::string> checkInitVal(nlohmann::ordered_json &initialValues, nlohmann::ordered_json &transitions) {
    std::vector<std::string> compNamesInit;
    std::vector<std::string> compNamesTransition;
    std::vector<std::string> diffs;

    // Get compNames in the initialValues json
    for (auto& initVal: initialValues.items()) {
        compNamesInit.push_back(initVal.key());
    }
    // Get compNames in the transitions json
    for (auto& transition: transitions.items()) {
        std::string flow = transition.key();
        // Remove whitespace
        flow.erase(remove(flow.begin(), flow.end(), ' '), flow.end());

        // We need to extract inComp, outComp from the string
        std::string inCompName;
        std::string outCompName;

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
                std::cout << "Compartment name must be string characters, not number" << std::endl;
            }
                // If s1 is number and s2 not number
            else if (std::strspn(s1.c_str(), "-.0123456789") == s1.size()) {
                inCompName = s2;
            }
                // If s2 is number
            else if (std::strspn(s2.c_str(), "-.0123456789") == s2.size()) {
                inCompName = s1;
            }
                // Else if both are not number
            else {
                std::cout << "Proportion must be a number" << std::endl;
            }
        }

        //======================== Manage the outCompName part ========================//
        if (flow.find(',') != std::string::npos) {
            // Make a vector outComps to store separated outCompName i.e. from "I1, I2, I3" to "I1", "I2", "I3"
            std::vector<std::string> outComps;
            std::stringstream outStrings(outCompName);
            std::string segment;
            while (std::getline(outStrings, segment, ',')) {
                compNamesTransition.push_back(segment);
            }
        } else {
            compNamesTransition.push_back(outCompName);
        }
        compNamesTransition.push_back(inCompName);
    }

    // Sort the two vectors and get unique compNameTransition
    std::sort(compNamesInit.begin(), compNamesInit.end());
    std::sort(compNamesTransition.begin(), compNamesTransition.end());
    compNamesTransition.erase(std::unique(compNamesTransition.begin(), compNamesTransition.end()), compNamesTransition.end());

    std::set_difference(compNamesTransition.begin(), compNamesTransition.end(),
                        compNamesInit.begin(), compNamesInit.end(),
                        std::inserter(diffs, diffs.begin()));

    return diffs;
}