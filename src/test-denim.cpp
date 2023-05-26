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
#include "ModelJSON.h"

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Distribution class") {
    DistributionConstant distribution(0.5);
    std::vector<double> inputVect{0.3, 0.5, 0.1, 0.1};
    size_t idx = 2;

    test_that("calcTransitionProbHelper()") {
        expect_true(distribution.calcTransitionProbHelper(inputVect, idx) == Approx(0.5).margin(0.001));
        expect_false(distribution.calcTransitionProbHelper(inputVect, idx) == Approx(1).margin(0.001));
    }
}

context("ModelJSON") {
    nlohmann::ordered_json j = nlohmann::ordered_json::parse("{  \"simulationDuration\": 10,  \"errorTolerance\": 0.001,  \"timeStep\": 0.01,  \"initialValues\": {\"S\": 999, \"I\": 1, \"R\": 0},  \"parameters\": {\"beta\": 0.12, \"N\": 1000},  \"transitions\": {  \"S -> I\": {\"distribution\": \"mathExpression\", \"expression\": \"beta * S * I / N\"},  \"I -> R\": {\"distribution\": \"gamma\", \"scale\": 3, \"shape\": 2}}}");

    ModelJSON modeljson(j["initialValues"], j["parameters"], j["transitions"]);
    auto model = modeljson.getModel();
    model->sortCompsByInputTransition();
    std::vector<std::string> compsOrder = {"S", "I", "R"};

    test_that("getCompsOrder()") {
      expect_true(model->getCompsOrder() == compsOrder);
    }
}

context("Compartment") {
    test_that("updateCompartment()") {
    }
}