#include "simulate.h"

Rcpp::DataFrame simulate(std::string inputPath) {

        // ========================== JSON input ==============================

    // Read a JSON input file to provide parameters
    // std::ifstream inputFile(inputPath);
    nlohmann::json input = nlohmann::json::parse(inputPath);
    // inputFile >> input;

    // Record execution time: https://stackoverflow.com/questions/21856025/getting-an-accurate-execution-time-in-c-micro-seconds
    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "Reading input file..." << "\n";

    // Initialize parameters
    Compartment::daysFollowUp = input["daysFollowUp"];
    Distribution::errorTolerance = input["errorTolerance"];
    if (!input["timeStep"].is_null()) {
        Distribution::timeStep = input["timeStep"];
    }
    for (std::string structure: input["modelStructure"]) {
        Model::modelStructure.push_back(structure);
    }
    for (std::string infComp: input["infectiousComps"]) {
        Model::infectiousComps.push_back(infComp);
    }

    // Initialize contactAssumption first because the contact will be generate following this order
    std::vector<std::shared_ptr<Contact>> allContacts;
    if (!input["contacts"].is_null()) {
        for (auto& contactConfig: input["contacts"]) {
            Contact::contactAssumption.push_back(contactConfig["contactType"]);
            auto contact = std::make_shared<Contact>(contactConfig["contactType"], contactConfig["contactClasses"], contactConfig["contactRates"]);
            allContacts.push_back(contact);
        }
    }

    // ====== Initialize the full model ======
    FullModel allModels(allContacts);

    // For each location:
    for (auto& modelConfig: input["models"]) {

        // Generate all compartments in this location
        std::vector<std::shared_ptr<Compartment>> allCompartments;
        for (auto& compConfig: modelConfig["compartments"]) {
            CompartmentJSON compJson(compConfig);
            allCompartments.push_back(compJson.getComp());
        }

        // Make model for this location
        auto myModel = std::make_shared<Model>(modelConfig["modelName"], modelConfig["transmissionRate"]);
        myModel->addCompsFromConfig(allCompartments);

        // Because all compartments had been created, we can connect the compartments now
        myModel->connectComp();

        // Check cycle, sort and calculate population size
        myModel->sortComps();
        myModel->calcPopulationSize();
        if (!allContacts.empty()) {
            myModel->sortModelGroupByAssumption(allContacts);
        }

        // Finally, add this model to the full model
        allModels.addModel(myModel);
    }

    // After adding all models, connect them
    allModels.connectModels();

    std::cout << "Simulating..." << "\n";

    // ======================== End JSON input ==============================

    // ==================== Construct and run model ==========================

    // BE CAUTIOUS: The order of the following two for loop is extremely important, at each iteration we want to
    // update location 1, then location 2, then move on to the next iteration. We never want to update a location
    // from iter 1 to iter 100 then continue to update the next location. So the "iter" for loop comes first, then
    // the "location" for loop
    for (size_t i {1}; i < Compartment::daysFollowUp; i++) {
        for (auto& myModel: allModels.getModels()) {
            myModel->update(i);
        }
    }

    // Display execution time
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    long long seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsedTime).count();
    std::cout << "Simulation completed, elapsed time: " << seconds << " seconds\n";

    // ================== End construct and run model ========================

    Rcpp::DataFrame df;
    std::vector<int> timeStep(Compartment::daysFollowUp);
    std::iota(std::begin(timeStep), std::end(timeStep), 0);
    df.push_back(timeStep, "Time");
    for (auto& model: allModels.getModels()) {
        std::string modelName {""};
        for (size_t i {0}; i < model->getModelGroup().size(); ++i) {
            if (i < (model->getModelGroup().size() - 1)) {
                modelName += model->getModelGroup()[i] + "_";
            } else if (i == (model->getModelGroup().size() - 1)) {
                modelName += model->getModelGroup()[i];
            }
        }
        for (auto& comp: model->getComps()) {
            std::string compName = comp->getName() + "_" + modelName;
            df.push_back(comp->getTotal(), compName);
        }
    }
    return df;
}


