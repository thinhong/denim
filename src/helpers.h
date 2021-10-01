//
// Created by thinh on 08/09/2021.
//

#ifndef MAIN_CPP_HELPERS_H
#define MAIN_CPP_HELPERS_H

#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "json.h"
#include "Model.h"

std::vector<std::string> extractCompNames(std::string element);
void viewModelStructure(std::shared_ptr<Model> model);
void viewModelUpdate(std::shared_ptr<Model> model, long iter);
std::vector<std::string> checkInitVal(nlohmann::ordered_json& initialValues, nlohmann::ordered_json& transitions);

#endif //MAIN_CPP_HELPERS_H
