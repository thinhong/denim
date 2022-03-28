#include "simcm.h"

Rcpp::DataFrame simcm(std::string inputPath) {

        // ========================== JSON input ==============================

    // Read a JSON input file to provide parameters
    nlohmann::ordered_json input;
    input = nlohmann::ordered_json::parse(inputPath);

    // Record execution time: https://stackoverflow.com/questions/21856025/getting-an-accurate-execution-time-in-c-micro-seconds
    auto startTime = std::chrono::high_resolution_clock::now();

    // Initialize parameters: errorTolerance, timeStep and daysFollowUp
    Distribution::errorTolerance = input["errorTolerance"];
    if (!input["timeStep"].is_null()) {
        Distribution::timeStep = input["timeStep"];
    }
    Compartment::timesFollowUp = static_cast<size_t>(static_cast<double>(input["simulationDuration"]) / Distribution::timeStep + 1);

    ModelJSON myModel(input["initialValues"], input["parameters"], input["transitions"]);

    // We sort the comps with order defined by input transition
    myModel.getModel()->sortCompsByInputTransition();
    myModel.getModel()->initAllComps();

    // Debug: view model structure
//    viewModelStructure(myModel.getModel());

    Rcpp::Rcout << "Simulating..." << "\n";

    // ======================== End JSON input ==============================

    // ==================== Construct and run model ==========================

    for (size_t i {1}; i < Compartment::timesFollowUp; i++) {
        myModel.getModel()->update(i);
        // Debug: view each time step update
//        viewModelUpdate(myModel.getModel(), i);
    }

    // Display execution time
    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    double seconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
    seconds /= 1000;
    Rcpp::Rcout << "Simulation completed, elapsed time: ";
    Rcpp::Rcout << std::fixed << std::setprecision(4) << seconds << " seconds\n";

    // ================== End construct and run model ========================

    Rcpp::DataFrame df;
    std::vector<int> timeStep(Compartment::timesFollowUp);
    std::iota(std::begin(timeStep), std::end(timeStep), 0);
    std::vector<double> actualTime;
    actualTime.clear();
    for (auto& time: timeStep) {
        actualTime.push_back(Distribution::timeStep * time);
    }
    df.push_back(actualTime, "Time");
    for (auto& comp: myModel.getModel()->getComps()) {
        std::string compName = comp->getCompName();
        df.push_back(comp->getCompTotal(), compName);
    }
    return df;
}


