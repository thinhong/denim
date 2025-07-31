#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>
#include "json.h"
#include "Compartment.h"
#include "Transition.h"
#include "ModelJSON.h"
#include "helpers.h"
#include <Rcpp.h>

Rcpp::DataFrame simcm(std::string inputPath);


