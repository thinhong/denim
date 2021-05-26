#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "json.h"
#include "Contact.h"
#include "Compartment.h"
#include "Model.h"
#include "FullModel.h"
#include "Distribution.h"
#include "TransitionProb.h"
#include "CompartmentJSON.h"
#include <chrono>
#include <Rcpp.h>

Rcpp::DataFrame simcm(std::string inputPath);


