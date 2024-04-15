#ifndef MAIN_CPP_DISTRIBUTIONPARAMETRIC_H
#define MAIN_CPP_DISTRIBUTIONPARAMETRIC_H

#include "Distribution.h"
#include <functional>

// Parent class for defined cdf distribution 
class DistributionParametric: public Distribution {    
protected:
    std::vector<double> transitionProb;
    void calcTransitionProb(std::function<double(double)> cdf_func);  
public:
    double getTransitionProb(size_t index) override;
};


#endif //MAIN_CPP_DISTRIBUTIONPARAMETRIC_H
