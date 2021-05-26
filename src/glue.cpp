#include <Rcpp.h>
#include "Contact.h"
#include "Distribution.h"
#include "CustomDistribution.h"
#include "DiscreteExponentialDistribution.h"
#include "DiscreteGammaDistribution.h"
#include "Compartment.h"
#include "Model.h"
#include "simcm.h"

using namespace Rcpp;

RCPP_MODULE(simcm) {
  function("simcm", &simcm)
  ;
}
