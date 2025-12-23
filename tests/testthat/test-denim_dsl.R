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
    "S -> V "= "2",
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
    beta = 1.2,
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


test_that("Make sure the DSL generated the expected model definition", {
  # ----- Set up ----
  initialValues <- c(S = 999, I = 1, R = 0, V = 0, IV = 0, D = 0) 
  
  transitions <- denim_dsl({ 
    S -> I = beta * S * (I + IV) / N 
    S -> V = d_exponential(0.01) 
    0.2 * I -> D = d_lognormal(2, 0.5) 
    0.8 * I -> R = d_gamma(IR_rate, IR_shape) 
    V -> IV = 0.2 * beta * V * (I + IV) / N 
    IV -> R = nonparametric(iv_r_dist) 
    IV -> D = d_weibull(scale = 2, shape = 1.5) 
  }) 
  
  parameters <- list( 
    beta = 0.9,  
    N = 1000,
    IR_rate = 1/3, IR_shape = 1.8,
    iv_r_dist = c(0, 0.15, 0.15,  0.05, 0.2, 0.2, 0.25)
  )
  
  simulationDuration <- 10
  timeStep <- 0.01
  
  # ----- Try parsing model ----
  mod <- newModel(transitions=transitions, initialValues = initialValues, 
                      parameters = parameters, 
                      simulationDuration=simulationDuration,
                      errorTolerance = 0.001)
  
  # make sure the transitions are captured properly
  expect_true(all(c(
    "S -> I", "S -> V", "0.2 * I -> D", "0.8 * I -> R",
    "V -> IV", "IV -> R", "IV -> R") %in% names(mod$transitions)))
  
  # ---- Make sure RHS is inferred properly ----
  # make sure all the RHS are properly converted to Transition object
  expect_true(
    all(
      sapply(
        names(mod$transitions), 
         \(trans){ 
           class(mod$transitions[[trans]])[1] == "Transition"
           }
      )
    )
  )
  
  # ---- Check the inferred transition type ----
  expect_true(all(
    sapply(
      c("S -> I", "V -> IV"), 
      \(trans){ 
        mod$transitions[[trans]]$transition == "mathExpression"
      })
  ))
  expect_equal(mod$transitions$`0.2 * I -> D`$transition, "lognormal")
  expect_equal(mod$transitions$`0.8 * I -> R`$transition, "gamma")
  expect_equal(mod$transitions$`S -> V`$transition,  "exponential")
  expect_equal(mod$transitions$`IV -> R`$transition,  "nonparametric")
  expect_equal(mod$transitions$`IV -> D`$transition,  "weibull")
  
  
  # ----- Check whether distributional parameters are inferred ------ 
  expect_true(is.numeric(mod$transitions$`0.2 * I -> D`$mu) & 
                is.numeric(mod$transitions$`0.2 * I -> D`$sigma))
  expect_true(is.numeric(mod$transitions$`0.8 * I -> R`$rate) & 
                is.numeric(mod$transitions$`0.8 * I -> R`$shape))
  # also make sure that the waiting time in nonparametric distribution is captured properly
  expect_true(is.numeric(mod$transitions$`IV -> R`$waitingTime))

  # ----- Make sure distributional parameters are parsed correctly -----
  expect_equal(mod$transitions$`0.8 * I -> R`$rate,
               parameters[["IR_rate"]])
  expect_equal(mod$transitions$`0.8 * I -> R`$shape,
               parameters[["IR_shape"]])
  expect_setequal(mod$transitions$`IV -> R`$waitingTime,
              parameters[["iv_r_dist"]])
})

test_that("Make sure the model definition is parsed to JSON correctly", {
  # ----- Set up ----
  transitions <- denim_dsl({
    S -> I = beta* S * (I + IV) / N
    S -> V = 2
    0.2 *  I -> D = d_lognormal(mu = 2, sigma = 1/5)
    0.8 *  I -> R = d_gamma(rate = IR_rate, shape = IR_shape)
    V -> IV = 0.1 *  beta * V * (I + IV) / N
    IV -> R = d_exponential(2)
  })
  initialValues <- c(S = 999, I = 1, R = 0, V = 0, IV = 0, D = 0)
  parameters <- c(
    beta = 1.2,
    N = 1000,
    IR_rate = 1/3, IR_shape = 1.8
  )
  simulationDuration <- 10
  timeStep <- 0.01
  
  mod <- newModel(transitions=transitions, initialValues = initialValues, 
                  parameters = parameters, 
                  simulationDuration=simulationDuration,
                  errorTolerance = 0.001)
  
  # Make sure lower level JSON parsing functions work properly
  modelToJson(mod)
  strip_ws <- \(x) {gsub("\\s+", "", x)}
  
  # ----- check newJsonKeyPair function
  ivKeyPairs <- newJsonKeyPair(names(initialValues), initialValues)
  pmKeyPairs <- newJsonKeyPair(names(parameters), parameters)
  json_simdur <- newJsonKeyPair("simulationDuration", simulationDuration)
  
  expect_setequal(strip_ws(ivKeyPairs),
                  strip_ws(c("\"S\":999","\"I\":1","\"R\":0","\"V\":0","\"IV\":0","\"D\":0")))
  expect_setequal(strip_ws(pmKeyPairs),
                  strip_ws(c("\"beta\":1.2","\"N\":1000","\"IR_rate\":0.333333333333333","\"IR_shape\":1.8")))
  expect_equal(json_simdur, '\"simulationDuration\": 10')
  
  # ---- Check JSON conversion for transition
  # math expression
  math_trans <- transitionToJson(mod$transitions$`0.2 * I -> D`)
  # distribution based transition
  dist_trans <- transitionToJson(mod$transitions$`IV -> R`)
  expect_equal(math_trans, 
               '{\"transition\": \"lognormal\", \"mu\": 2, \"sigma\": 0.2, \"dist_init\": 0}')
  expect_equal(
    dist_trans,
    '{\"transition\": \"exponential\", \"rate\": 2, \"dist_init\": 0}'
  )

  
  # ----- check the function to make JSON object
  ivObj <- newJsonObject(ivKeyPairs, inline = TRUE)
  pmObj <- newJsonObject(pmKeyPairs, inline = TRUE)
  expect_equal(
    ivObj, 
    '{\"S\": 999, \"I\": 1, \"R\": 0, \"V\": 0, \"IV\": 0, \"D\": 0}')
  expect_equal(
    pmObj, 
    '{\"beta\": 1.2, \"N\": 1000, \"IR_rate\": 0.333333333333333, \"IR_shape\": 1.8}')
  
  # ----- check the function to make nested JSON object
  expect_equal(
    newJsonNestedObject("initialValues", ivObj),
    '\"initialValues\": {\"S\": 999, \"I\": 1, \"R\": 0, \"V\": 0, \"IV\": 0, \"D\": 0}'
  )
  expect_equal(
    newJsonNestedObject("parameters", pmObj),
    '\"parameters\": {\"beta\": 1.2, \"N\": 1000, \"IR_rate\": 0.333333333333333, \"IR_shape\": 1.8}'
  )

})
