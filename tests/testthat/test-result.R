test_that("Compare gamma distribution with desolve", {
    # --- Transition def for denim
    transitions <- list(
    "S -> I" = d_gamma(3, 2)
    )
    parameters <- c(scale = 3, shape=2) 
    initialValues <- c(S = 999, S1 = 999, S2 = 0, I = 0)

    # --- Transition def for sim
    transition_func <- function(t, state, param){
        with(as.list( c(state, param) ), {
            rate = 1/scale
            # rate of change
            dS1 = -rate*S1
            dS2 = rate*S1 - rate*S2
            dS = dS1 + dS2
            dI = rate*S2
            list(c(dS, dS1, dS2,  dI))
        })
    }

    # --- compute result using both packages
    mod <- sim(transitions = transitions, initialValues = initialValues, parameters = parameters, simulationDuration = 10, timeStep = 1)
    timestep <- seq(0, 10, 1)
    ode_mod <- ode(y = initialValues, times = timestep, parms = parameters, func = transition_func)

    tolerance <- 0.0001
    abs_diff <- abs(mod[, c("Time", "S", "I")] - ode_mod[, c("time", "S", "I")])

    expect_true(all(abs_diff <= tolerance))
})

test_that("Compare exponential distribution with desolve", {
    # --- Transition def for denim
    transitions <- list(
    "S -> I" = d_exponential(0.2)
    )
    parameters <- c(rate=0.2) 
    initialValues <- c(S = 999, I = 0)

    # --- Transition def for sim
    transition_func <- function(t, state, param){
    with(as.list( c(state, param) ), {
        # rate 
        dS = -rate*S
        dI = rate*S
        list(c(dS,  dI))
    })
    }

    # --- compute result using both packages
    mod <- sim(transitions = transitions, initialValues = initialValues, parameters = parameters, simulationDuration = 10, timeStep = 1)
    timestep <- seq(0, 10, 1)
    ode_mod <- ode(y = initialValues, times = timestep, parms = parameters, func = transition_func)

    tolerance <- 0.001
    abs_diff <- abs(mod[, c("Time", "S", "I")] - ode_mod[, c("time", "S", "I")])

    expect_true(all(abs_diff <= tolerance))
})