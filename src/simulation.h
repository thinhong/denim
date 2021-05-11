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
#include "FileCSV.h"
#include "CompartmentJSON.h"
#include <filesystem>
#include <chrono>

void simulation(std::string inputPath, std::string outputFolder);


