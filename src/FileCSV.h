//
// Created by thinh on 09/02/2021.
//

#ifndef MAIN_CPP_FILECSV_H
#define MAIN_CPP_FILECSV_H

#include <string>
#include <memory>
#include "File.h"
#include "Model.h"

class FileCSV: public File {
private:
    Model* model;
    std::string filePath;
    std::string fileName;
public:
    FileCSV(std::string filePath, std::string fileName, Model* model);
    void writeFile() override;
};


#endif //MAIN_CPP_FILECSV_H
