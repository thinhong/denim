//
// Created by thinh on 09/02/2021.
//

#include <fstream>
#include <iomanip>
#include <filesystem>
#include "FileCSV.h"

FileCSV::FileCSV(std::string filePath, std::string fileName, Model* model) {
    this->filePath = filePath;
    this->fileName = fileName;
    this->model = model;
}

void FileCSV::writeFile() {

    // Make sure that the path is valid
    std::string fullPath;
    if (filePath.back() == '/') {
        fullPath = filePath + fileName;
    } else {
        fullPath = filePath + "/" + fileName;
    }

    // If the directory is not exist, it will create a new directory with that name
    if(!std::filesystem::is_directory(filePath) || !std::filesystem::exists(filePath)) {
        std::cout << "Create directory " << filePath << " to store output..." << "\n";
        std::filesystem::create_directories(filePath);
    }

    std::ofstream myFile(fullPath);
    myFile << std::fixed << std::setprecision(precision);
    if (myFile.is_open()) {
        myFile << "Time,";
        for (size_t j {0}; j < model->getComps().size(); ++j) {
            std::string groupName;
            for (size_t i {0}; i < model->getModelGroup().size(); ++i) {
                if (i < (model->getModelGroup().size() - 1)) {
                    groupName += model->getModelGroup()[i] + "_";
                } else if (i == (model->getModelGroup().size() - 1)) {
                    groupName += model->getModelGroup()[i];
                }
            }
            if (j == model->getComps().size() - 1) {
                myFile << model->getComps()[j]->getName() << "_" << groupName << "\n";
            } else {
                myFile << model->getComps()[j]->getName() << "_" << groupName << ",";
            }
        }
        for (size_t i {0}; i < model->getComps()[0]->getTotal().size(); ++i) {
            myFile << i << ",";
            for (size_t j {0}; j < model->getComps().size(); ++j) {
                if (j == model->getComps().size() - 1) {
                    myFile << model->getComps()[j]->getTotal()[i] << "\n";
                } else {
                    myFile << model->getComps()[j]->getTotal()[i] << ",";
                }
            }
        }
        myFile.close();
        std::cout << "Successfully write simulation output into file: " << fullPath << std::endl;
    } else {
        std::cerr << "Unable to write file: " << fullPath << std::endl;
    }
}