//
// Created by thinh on 05/02/2021.
//

#include "Distribution.h"


std::string Distribution::getDistName(){
    return this->distName;
};

double Distribution::getProbDist(size_t index){
    return 1;
}; 

size_t Distribution::getMaxDay(){
    return this->maxDay;
}

Distribution::~Distribution() {}