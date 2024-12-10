test_that("Compare denim and deSolve", {
    # --- Transition def for denim
    transitions <- list(
    "S -> I" = d_exponential(0.2),
    "I -> R" = d_gamma(3, 2)
    )
    parameters <- c(rate = 0.2, scale = 3, shape=2) 
    initialValues <- c(S = 999, I = 1, I1 = 1, I2=0, R=0)

    # --- Transition def for deSolve
    transition_func <- function(t, state, param){
    with(as.list( c(state, param) ), {
        gamma_rate = 1/scale
        dS = -rate*S
        dI1 = rate*S - gamma_rate*I1
        dI2 = gamma_rate*I1 - gamma_rate*I2
        dI =  dI1 + dI2
        dR = gamma_rate*I2
        list(c(dS, dI, dI1, dI2, dR))
    })
    }

    # --- Timestep definition
    simulationDuration <- 20 
    timestep <- 0.0005 # use small timestep to compare with deSolve which has continuous timestep

    # --- Simulate using both package
    mod <- sim(transitions = transitions, initialValues = initialValues, parameters = parameters, simulationDuration = simulationDuration, timeStep = timestep)
    ode_mod <- deSolve::ode(y = initialValues, times = seq(0, simulationDuration, timestep), parms = parameters, func = transition_func)


    ode_mod <- as.data.frame(ode_mod)

    tolerance <- 0.1
    abs_diff <- abs(mod[, c("Time", "S", "I")] - ode_mod[, c("time", "S", "I")])
    
    expect_true(all(abs_diff <= tolerance))
    
    # test plot function does not throw error
    expect_no_error(plot(mod))
})

