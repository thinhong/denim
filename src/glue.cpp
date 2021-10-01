#include <Rcpp.h>
#include "simcm.h"

using namespace Rcpp;

RCPP_MODULE(simcm) {
  function("simcm", &simcm)
  ;
}
