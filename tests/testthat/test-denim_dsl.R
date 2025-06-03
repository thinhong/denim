test_that("Compare result for model definition using DSL vs list", {
  # --- model using DSL
  model_dsl <- denim_dsl({
    S -> I = beta* S * (I + IV) / N
    S -> V = 2
    0.1 * I -> D=d_lognormal(mu = 2, sigma = 1/5)
    0.9 * I -> R = d_gamma(rate = 1/3, shape = 1.8)
    V -> IV = 0.1 * beta * V * (I + IV) / N
    IV -> R = d_exponential(2)
  })
  
  # --- model as list
  model_list <- list(
    "S -> I" = "beta* S * (I + IV) / N",
    "S -> V "= 2,
    "0.1 * I -> D" = d_lognormal(mu = 2, sigma = 1/5),
    "0.9 * I -> R" = d_gamma(rate = 1/3, shape = 1.8),
    "V -> IV" = "0.1 * beta * V * (I + IV) / N",
    "IV -> R" = d_exponential(2)
  )
  
  initialValues <- c(
    S = 999, 
    I = 1, 
    R = 0,
    V = 0,
    IV = 0,
    D = 0
  )
  
  parameters <- c(
    beta = 0.12,
    N = 1000
  )
  
  simulationDuration <- 10
  timeStep <- 0.01
  
  # making sure there is no error with DSL or list approach
  expect_no_error(
    {
      sim(
        transitions = model_dsl,
        initialValues = initialValues,
        parameters = parameters,
        simulationDuration = simulationDuration,
        timeStep = timeStep
      )
      sim(
          transitions = model_list,
          initialValues = initialValues,
          parameters = parameters,
          simulationDuration = simulationDuration,
          timeStep = timeStep
        )
    }
  )
  
  mod_dsl <- sim(
    transitions = model_dsl,
    initialValues = initialValues,
    parameters = parameters,
    simulationDuration = simulationDuration,
    timeStep = timeStep
  )
  mod_list <- sim(
      transitions = model_list,
      initialValues = initialValues,
      parameters = parameters,
      simulationDuration = simulationDuration,
      timeStep = timeStep
    )
  
  tolerance <- 0.01
  abs_diff <- round(abs(mod_dsl[, c("S", "I", "R")] - mod_list[, c("S", "I", "R")]), 3)
  expect_true(all(abs_diff <= tolerance))
})
