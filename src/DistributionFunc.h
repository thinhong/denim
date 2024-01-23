#ifndef MAIN_CPP_DISTRIBUTIONFUNC_H
#define MAIN_CPP_DISTRIBUTIONFUNC_H

#include "Distribution.h"

// Parent class for defined cdf distribution 
class DistributionFunc: public Distribution {    
protected:
    size_t maxDay {0};
    std::vector<double> transitionProb;
    void calcTransitionProb(std::function<double(int)> cdf_func);  
public:
    double getTransitionProb(size_t index) override;
};


#endif //MAIN_CPP_DISTRIBUTIONFUNC_H
