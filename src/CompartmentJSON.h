//
// Created by thinh on 22/02/2021.
//

#ifndef MAIN_CPP_COMPARTMENTJSON_H
#define MAIN_CPP_COMPARTMENTJSON_H

#include "json.h"
#include <memory>
#include "Compartment.h"
#include "TransitionProb.h"
#include "DiscreteGammaDistribution.h"
#include "DiscreteWeibullDistribution.h"
#include "DiscreteExponentialDistribution.h"
#include "CustomDistribution.h"

class CompartmentJSON {
private:
    std::shared_ptr<Compartment> comp;
public:
    explicit CompartmentJSON(nlohmann::json& jsonNode);
    std::shared_ptr<Compartment> getComp();
    nlohmann::json compToJSON();
};


#endif //MAIN_CPP_COMPARTMENTJSON_H
