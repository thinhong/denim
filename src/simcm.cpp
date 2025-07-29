#include "simcm.h"

Rcpp::DataFrame simcm(std::string inputPath) {

        // ========================== JSON input ==============================

    // Read a JSON input file to provide parameters
    nlohmann::ordered_json input;
    input = nlohmann::ordered_json::parse(inputPath);

    // Initialize parameters: errorTolerance, timeStep and daysFollowUp
    Distribution::errorTolerance = input["errorTolerance"];
    if (!input["timeStep"].is_null()) {
        Distribution::timeStep = input["timeStep"];
    }
    Compartment::timesFollowUp = static_cast<size_t>(static_cast<double>(input["simulationDuration"]) / Distribution::timeStep + 1);

    ModelJSON myModel(input["initialValues"], input["parameters"], input["transitions"]);

    // We sort the comps with order defined by input transition
    myModel.getModel()->sortCompsByInputTransition();

    // Debug: view model structure
//    viewModelStructure(myModel.getModel());

    // ======================== End JSON input ==============================

    // ==================== Construct and run model ==========================

    for (size_t i {1}; i < Compartment::timesFollowUp; i++) {
        myModel.getModel()->update(i);
        // Debug: view each time step update
//        viewModelUpdate(myModel.getModel(), i);
    }

    // ================== End construct and run model ========================

    Rcpp::DataFrame df;
    // std::vector<int> timeStep(Compartment::timesFollowUp);
    // std::iota(std::begin(timeStep), std::end(timeStep), 0);
    // std::vector<double> actualTime;
    // actualTime.clear();
    // for (auto& time: timeStep) {
    //     actualTime.push_back(Distribution::timeStep * time);
    // }
    // TODO: test new code with removed redundant loop
    std::vector<double> actualTime;
    actualTime.reserve(Compartment::timesFollowUp);
    for (int i = 0; i < Compartment::timesFollowUp; ++i) {
        actualTime.push_back(Distribution::timeStep * i);
    }
    df.push_back(actualTime, "Time");
    for (auto& comp: myModel.getModel()->getComps()) {
        std::string compName = comp->getCompName();
        df.push_back(comp->getCompTotal(), compName);
    }
    return df;
}


