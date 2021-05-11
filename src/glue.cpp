#include <Rcpp.h>
#include "Contact.h"
#include "Distribution.h"
#include "CustomDistribution.h"
#include "DiscreteExponentialDistribution.h"
#include "DiscreteGammaDistribution.h"
#include "Compartment.h"
#include "Model.h"
#include "simulation.h"

using namespace Rcpp;

// RCPP_EXPOSED_CLASS(CustomDistribution)
// RCPP_EXPOSED_AS(std::shared_ptr<CustomDistribution>)
// RCPP_EXPOSED_WRAP(std::shared_ptr<CustomDistribution>)
// RCPP_EXPOSED_CLASS(Compartment)
// RCPP_EXPOSED_AS(std::shared_ptr<Compartment>)
// RCPP_EXPOSED_WRAP(std::shared_ptr<Compartment>)
// RCPP_EXPOSED_AS(std::weak_ptr<Compartment>)
// RCPP_EXPOSED_WRAP(std::weak_ptr<Compartment>)

// Expose classes
// Contact
RCPP_MODULE(Contact) {
  class_<Contact>("Contact")
  .constructor<std::string, std::vector<std::string>, std::vector<std::string>>()

  .property("contactType", &Contact::getContactType)
  ;
}

// CustomDistribution
RCPP_MODULE(CustomDistribution){
  class_<CustomDistribution>("CustomDistribution")
  .constructor<std::vector<double>>()

  // Private attributes
  .property("distName", &CustomDistribution::getDistName)
  .property("waitingTime", &CustomDistribution::getWaitingTime)
  .property("maxDay", &CustomDistribution::getMaxDay)

  // Method
  // Because getTransitionProb(size_t index) take an argument, it cannot act as a getter for
  // attribute transitionProb, we need to expose it as a method
  .method("getTransitionProb", &CustomDistribution::getTransitionProb)
  ;
}

// DiscreteExponentialDistribution
RCPP_MODULE(DiscreteExponentialDistribution){
  class_<DiscreteExponentialDistribution>("DiscreteExponentialDistribution")
  .constructor<double>()

  .property("distName", &DiscreteExponentialDistribution::getDistName)
  .property("rate", &DiscreteExponentialDistribution::getRate)
  .property("maxDay", &DiscreteExponentialDistribution::getMaxDay)

  .method("getTransitionProb", &DiscreteExponentialDistribution::getTransitionProb)
  ;
}

// DiscreteGammaDistribution
RCPP_MODULE(DiscreteGammaDistribution){
  class_<DiscreteGammaDistribution>("DiscreteGammaDistribution")
  .constructor<double, double>()

  .property("distName", &DiscreteGammaDistribution::getDistName)
  .property("maxDay", &DiscreteGammaDistribution::getMaxDay)

  .method("getTransitionProb", &DiscreteGammaDistribution::getTransitionProb)
  ;
}
// 
// // Compartment
// RCPP_MODULE(Compartment){
//   class_<Compartment>("Compartment")
//   .constructor<std::string, double, CustomDistribution>()
// 
//   .property("name", &Compartment::getName)
//   .property("subCompartmentValues", &Compartment::getSubCompartmentValues)
// 
//   .method("getLinkedCompartment", &Compartment::getLinkedCompartment)
//   ;
// }
// 
// // [[Rcpp::export]]
// SEXP CompartmentSharedPtr() {
//   Compartment* pd = new Compartment();
//   Rcpp::XPtr<Compartment> ptr(pd);
//   return ptr;
// }
// 
// // Model
// RCPP_MODULE(Model) {
//   class_<Model>("Model")
//   .constructor<std::vector<std::string>, double>()
//   
//   .property("modelName", &Model::getModelGroup)
//   ;
// }

RCPP_MODULE(simulation) {
  function("simulation", &simulation)
  ;
}
