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
#include "TransitionConstant.h"
#include "TransitionLogNormal.h"
#include "ModelJSON.h"


// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Transition class") {
  std::vector<double> inputVect{0.3, 0.5, 0.1, 0.1};
  TransitionNonparametricDist distribution(inputVect);
  size_t idx = 2;

  test_that("getTransititionProb") {
    expect_true(distribution.getTransitionProb(idx) == Approx(0.5).margin(0.001));
    expect_false(distribution.getTransitionProb(idx) == Approx(1).margin(0.001));
  }

  test_that("maxDay"){
    expect_true(distribution.getMaxDay() == inputVect.size());
  }
  
  test_that("getWaitingTime"){
    expect_true(distribution.getWaitingTime() == inputVect);
  }
}

context("Lognormal distribution") {
  TransitionLogNormal distr(1, 0.25);
  std::shared_ptr<Transition> test_polymorphism; 
  test_polymorphism = std::make_shared<TransitionLogNormal>(1, 0.25);
  
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
  TransitionDiscreteGamma distr(1, 3);

  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(4) == Approx(0.4764).margin(0.01));
  }
  
  test_that("getRate") {
    expect_true(distr.getRate() == Approx(1).margin(0.01));
  }
  
  test_that("getShape") {
    expect_true(distr.getShape() == Approx(3).margin(0.01));
  }
}

context("Exponential distribution") {
  TransitionDiscreteExponential distr(0.5);

  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(2) == Approx(0.3934).margin(0.01));
  }
  
  test_that("getRate") {
    expect_true(distr.getRate() == Approx(0.5).margin(0.01));
  }
}

context("Transition Constant"){
  TransitionConstant distr(10);
  
  test_that("getTransitionProb"){
    expect_true(distr.getTransitionProb(3) == 10);
  }
}

context("Weibull distribution") {
  TransitionDiscreteWeibull distr(3, 5);

  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(2) == Approx(0.5803).margin(0.01));
  }
  
  test_that("getScale") {
    expect_true(distr.getScale() == Approx(3).margin(0.01));
  }
  
  test_that("getShape") {
    expect_true(distr.getShape() == Approx(5).margin(0.01));
  }
}

context("Math Expression"){
  std::string expr = "beta * S * I / N";
  
  TransitionMathExpression distr(expr);
  
  test_that("getTransitionProb"){
    expect_true(distr.getTransitionProb(1) == Approx(1.0).margin(0.0001));
  }
}



context("Transition prob") {
  TransitionProb distr(0.6);
  
  test_that("getTransitionProb") {
    expect_true(distr.getTransitionProb(2) == Approx(0.6).margin(0.01));
  }
}

context("Testing multinomial transition"){
  
  nlohmann::ordered_json j = nlohmann::ordered_json::parse("{\n  \"simulationDuration\": 10,\n  \"errorTolerance\": 0.001,\n  \"timeStep\": 1,\n  \"initialValues\": {\"S\": 1000, \"I\": 0, \"V\": 0},\n  \"parameters\": {\"transitionProb\": 0.2, \"mu\": 2, \"sigma\": 0.5},\n  \"transitions\": {\n  \"S -> I, V\": {\"transition\": \"multinomial\", \"probabilities\": [0.9, 0.1]},\n  \"S -> I\": {\"transition\": \"transitionProb\", \"transitionProb\": 0.2},\n  \"S -> V\": {\"transition\": \"lognormal\", \"mu\": 2, \"sigma\": 0.5}\n}\n}");
  ModelJSON modeljson(j["initialValues"], j["parameters"], j["transitions"]);
  auto model = modeljson.getModel();
  
  // map to convert compartment name to int per C++ requirement for switch case
  const static std::unordered_map<std::string,int> name_to_case{
    {"S",1},
    {"I",2},
    {"V",3}
  };
  
  // Set timestep and test values for the first iteration
  Transition::timeStep = j["timeStep"];
  model->update(1);
  
  test_that("update()"){
    // test updated value for each compartment
    for (auto &comp: model -> getComps()){
      switch (name_to_case.at(comp->getCompName())){
      case 1: 
        // check value for compartment S 
        expect_true(comp->getCompTotal()[1] == Approx(819.996).margin(0.01));
        break;
      case 2:
        // check value for compartment I 
        expect_true(comp->getCompTotal()[1] == Approx(180).margin(0.001));
        break;
      case 3:
        // check value for compartment V
        expect_true(comp->getCompTotal()[1] == Approx(0.003167).margin(0.001e-03));
        break;
      }
    }
  }
}


context("Testing distributions in model"){
  nlohmann::ordered_json j = nlohmann::ordered_json::parse("{  \"simulationDuration\": 10,\"errorTolerance\": 0.001,  \"timeStep\": 1,  \"initialValues\": {\"S\": 999, \"I\": 1, \"R\": 0, \"V\": 0, \"D\": 0},  \"parameters\": {\"N\": 1000},  \"transitions\": {  \"S -> I\": {\"transition\": \"nonparametric\", \"waitingTime\": [0.1, 0.2, 0.5, 0.2]},  \"S -> V\": {\"transition\": \"constant\", \"constant\": 2},  \"0.1 * I -> D\": {\"transition\": \"lognormal\", \"mu\": 2, \"sigma\": 0.5},  \"0.9 * I -> R\": {\"transition\": \"gamma\", \"rate\": 1/3, \"shape\": 2}}}");


  ModelJSON modeljson(j["initialValues"], j["parameters"], j["transitions"]);
  auto model = modeljson.getModel();
  model->sortCompsByInputTransition();
  std::vector<std::string> compsOrder = {"S", "I", "V", "D", "R"};

  test_that("complexGetCompsOrder") {
    expect_true(model->getCompsOrder() == compsOrder);
  }
  
  // map to convert compartment name to int per C++ requirement for switch case
  const static std::unordered_map<std::string,int> name_to_case{
    {"S",1},
    {"I",2},
    {"V",3},
    {"D",4},
    {"R",5}
  };
  
  // Set timestep and test values for the first iteration
  Transition::timeStep = j["timeStep"];
  model->update(1);
  // making sure update is working
  test_that("update"){
    // test updated value for each compartment
    for (auto &comp: model -> getComps()){
      switch (name_to_case.at(comp->getCompName())){
      case 1: 
        // check value for compartment S 
        expect_true(comp->getCompTotal()[1] == Approx(897.1).margin(0.01));
        break;
      case 2:
        // check value for compartment I 
        expect_true(comp->getCompTotal()[1] == Approx(100.86).margin(0.001));
        break;
      case 3:
        // check value for compartment V
        expect_true(comp->getCompTotal()[1] == Approx(2).margin(0.001e-03));
        break;
      case 4:
        // check value for compartment V
        expect_true(comp->getCompTotal()[1] == Approx(3.167e-06).margin(0.001e-06));
        break;
      case 5:
        // check value for compartment V
        expect_true(comp->getCompTotal()[1] == Approx(0.04016).margin(0.001e-03));
        break;
      }
    }
  }
}



context("Model JSON conversion") {
    nlohmann::ordered_json j = nlohmann::ordered_json::parse("{  \"simulationDuration\": 10,\"errorTolerance\": 0.001,  \"timeStep\": 0.01,  \"initialValues\": {\"S\": 999, \"I\": 1, \"R\": 0},  \"parameters\": {\"beta\": 0.12, \"N\": 1000},  \"transitions\": {  \"S -> I\": {\"transition\": \"mathExpression\", \"expression\": \"beta * S * I / N\"},  \"I -> R\": {\"transition\": \"gamma\", \"rate\": 1/3, \"shape\": 2}}}");

    ModelJSON modeljson(j["initialValues"], j["parameters"], j["transitions"]);
    auto model = modeljson.getModel();
    model->sortCompsByInputTransition();
    std::vector<std::string> compsOrder = {"S", "I", "R"};

    test_that("getCompsOrder()") {
      expect_true(model->getCompsOrder() == compsOrder);
    }
    
    test_that("getIndex"){
      expect_true(model->getIndex(model->getComps()[0]) == 0);
    }

    // Set timestep and test values for the first iteration
    Transition::timeStep = j["timeStep"];
    model->update(1);
    // map to convert compartment name to int per C++ requirement for switch case
    const static std::unordered_map<std::string,int> name_to_case{
      {"S",1},
      {"I",2},
      {"R",3}
    };
    
    test_that("compGetters"){
      model -> getComps()[0] -> getInCompartments();
      model -> getComps()[0] -> getOutCompartments();
      model -> getComps()[0] -> getOutCompartmentNames();
      model -> getComps()[0] -> getOutWeights();
      model -> getComps()[0] -> getOutValues();
      model -> getComps()[0] -> getSubCompartmentValues();
      
      expect_true(model -> getComps()[0] -> findOutCompPosition("N") == 0);
    }
    
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
    
    test_that("editOutTransition()"){
      std::shared_ptr<Transition> distr = std::make_shared<TransitionDiscreteWeibull>(3, 5);
      model -> getComps()[0] -> editOutTransition("I", distr);
    }

}


