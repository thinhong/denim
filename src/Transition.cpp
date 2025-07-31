//
// Created by thinh on 05/02/2021.
//

#include "Transition.h"


std::string Transition::getDistName(){
    return this->distName;
};

double Transition::getProbDist(size_t index){
    return 1;
}; 

size_t Transition::getMaxDay(){
    return this->maxDay;
}

Transition::~Transition() {}