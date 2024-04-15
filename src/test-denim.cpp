/*
 * This file uses the Catch unit testing library, alongside
 * testthat's simple bindings, to test a C++ function.
 *
 * For your own packages, ensure that your test files are
 * placed within the `src/` folder, and that you include
 * `LinkingTo: testthat` within your DESCRIPTION file.
 */

// All test files should include the <testthat.h> header file.
#include <testthat.h>

// denim headers
#include "DistributionConstant.h"
#include "DistributionLogNormal.h"
#include "ModelJSON.h"


// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Distribution class") {
  std::vector<double> inputVect{0.3, 0.5, 0.1, 0.1};
  DistributionNonparametric distribution(inputVect);
  size_t idx = 2;

  test_that("getTransititionProb") {
    expect_true(distribution.getTransitionProb(idx) == Approx(0.5).margin(0.001));
    expect_false(distribution.getTransitionProb(idx) == Approx(1).margin(0.001));
  }

  test_that("maxDay"){
    expect_true(distribution.getMaxDay() == inputVect.size());
  }
}

context("Lognormal distribution") {
  DistributionLogNormal distr(1, 0.25);
  std::shared_ptr<Distribution> test_polymorphism; 
  test_polymorphism = std::make_shared<DistributionLogNormal>(1, 0.25);
  
  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(2) == Approx(0.6106).margin(0.01));
  }

  test_that("maxDay") {
    expect_true(distr.getMaxDay() > 1);
  }

  test_that("polymorphism") {
    expect_true(distr.getTransitionProb(2) == test_polymorphism->getTransitionProb(2));
  }
}

context("Gamma distribution") {
  DistributionDiscreteGamma distr(1, 3);

  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(4) == Approx(0.4764).margin(0.01));
  }
}

context("Exponential distribution") {
  DistributionDiscreteExponential distr(0.5);

  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(2) == Approx(0.3934).margin(0.01));
  }
}

context("Weibull distribution") {
  DistributionDiscreteWeibull distr(3, 5);

  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(2) == Approx(0.5803).margin(0.01));
  }
}

context("Model and JSON conversion") {
    nlohmann::ordered_json j = nlohmann::ordered_json::parse("{  \"simulationDuration\": 10,  \"errorTolerance\": 0.001,  \"timeStep\": 0.01,  \"initialValues\": {\"S\": 999, \"I\": 1, \"R\": 0},  \"parameters\": {\"beta\": 0.12, \"N\": 1000},  \"transitions\": {  \"S -> I\": {\"distribution\": \"mathExpression\", \"expression\": \"beta * S * I / N\"},  \"I -> R\": {\"distribution\": \"gamma\", \"scale\": 3, \"shape\": 2}}}");

    ModelJSON modeljson(j["initialValues"], j["parameters"], j["transitions"]);
    auto model = modeljson.getModel();
    model->sortCompsByInputTransition();
    std::vector<std::string> compsOrder = {"S", "I", "R"};

    test_that("getCompsOrder()") {
      expect_true(model->getCompsOrder() == compsOrder);
    }

    // Set timestep and test values for the first iteration
    Distribution::timeStep = j["timeStep"];
    model->update(1);
    // map to convert compartment name to int per C++ requirement for switch case
    const static std::unordered_map<std::string,int> name_to_case{
      {"S",1},
      {"I",2},
      {"R",3}
    };
    test_that("update()"){
      // test updated value for each compartment
      for (auto &comp: model -> getComps()){
        switch (name_to_case.at(comp->getCompName())){
          case 1: 
            // check value for compartment S 
            expect_true(comp->getCompTotal()[1] == Approx(998.88).margin(0.01));
            break;
          case 2:
            // check value for compartment I 
            expect_true(comp->getCompTotal()[1] == Approx(1.1198).margin(0.001));
            break;
          case 3:
            // check value for compartment R
            expect_true(comp->getCompTotal()[1] == Approx(5.543e-06).margin(0.001e-06));
            break;
        }
      }
    }
}
