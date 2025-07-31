//
// Created by thinh on 22/07/2021.
//

#ifndef MAIN_CPP_MODELJSON_H
#define MAIN_CPP_MODELJSON_H

#include <cstring>
#include <memory>
#include <sstream>
#include "json.h"
#include "Compartment.h"
#include "Model.h"
#include "myProb.h"
#include "TransitionNonparametricDist.h"
#include "TransitionMathExpression.h"
#include "TransitionConstant.h"
#include "TransitionProb.h"
#include "TransitionDiscreteGamma.h"
#include "TransitionDiscreteWeibull.h"
#include "TransitionDiscreteExponential.h"
#include "TransitionLogNormal.h"

class ModelJSON {
private:
    std::shared_ptr<Model> model;
public:
    ~ModelJSON() {
//        std::cout << "ModelJSON destructor called." << std::endl;
    }
    explicit ModelJSON(nlohmann::ordered_json& initialValues, nlohmann::ordered_json& parameters, nlohmann::ordered_json& transitions);
    std::shared_ptr<Model> getModel();
};


#endif //MAIN_CPP_MODELJSON_H
