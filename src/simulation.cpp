#include "simulation.h"

void simulation(std::string inputPath, std::string outputFolder) {

        // ========================== JSON input ==============================

    // Read a JSON input file to provide parameters
    std::ifstream inputFile(inputPath);
    nlohmann::json input;
    inputFile >> input;

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
    if (!input["contactAssumption"].is_null()) {
        for (std::string assumption: input["contactAssumption"]) {
            Contact::contactAssumption.push_back(assumption);
        }
        // Vector allContacts contains contacts sorted by contactAssumption
        for (std::string contactType: Contact::contactAssumption) {
            for (auto& contactConfig: input["contacts"]) {
                std::string currentContactType = contactConfig["contactType"];
                if (currentContactType == contactType) {
                    auto contact = std::make_shared<Contact>(contactConfig["contactType"], contactConfig["contactClasses"], contactConfig["contactRates"]);
                    allContacts.push_back(contact);
                }
            }
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

    // ========================= Write output ================================
//    // Create json object to store all input parameters
//    nlohmann::json writeConfig;
//    writeConfig["daysFollowUp"] = Compartment::daysFollowUp;
//    writeConfig["errorTolerance"] = Distribution::errorTolerance;
//    writeConfig["populationSize"] = myModel.getPopulationSize();
//    writeConfig["transmissionRate"] = transmissionRate;
//    writeConfig["infectiousComps"] = infectiousComps;
//    for (auto i: myModel.getComps()) {
//        CompartmentJSON jsonNode;
//        jsonNode.compToJSON(i);
//        writeConfig["compartments"].push_back(jsonNode.getJsonNode());
//    }
//    std::ofstream myFile("/home/thinh/Downloads/config2.json");
//    if (myFile.is_open()) {
//        myFile << writeConfig;
//        myFile.close();
//        std::cout << "Successfully written input information into file: /home/thinh/Downloads/config2.json" <<
//        std::endl;
//    } else {
//        std::cout << "Unable to write config log file" << std::endl;
//    }

    // Write output to CSV file
    for (auto& model: allModels.getModels()) {
        Model* pModel = &(*model);
        std::string outputFileName;
        for (size_t i {0}; i < model->getModelGroup().size(); ++i) {
            if (i < (model->getModelGroup().size() - 1)) {
                outputFileName += model->getModelGroup()[i] + "_";
            } else if (i == (model->getModelGroup().size() - 1)) {
                outputFileName += model->getModelGroup()[i];
            }
        }
        outputFileName += ".csv";
        FileCSV file(outputFolder, outputFileName, pModel);
        file.writeFile();
    }
}


